#include "humanPlayer.h"
#include "sounds.h"

humanPlayer::humanPlayer(string nombreEnPantalla, string nombreVehiculo, string nombreMaterial, Vector3 posicionSalida, 
                         SceneManager* sceneMgr, DynamicsWorld* world, size_t laps, size_t checksPerLap,canalesSonidoCoche canalesSonido,
                         void* groundObject, size_t id, bool buildForMe)
                         : _nombreEnPantalla(nombreEnPantalla), _nombreVehiculo(nombreVehiculo), _nombreMaterial(nombreMaterial),
                           _posicionSalida(posicionSalida), _sceneMgr(sceneMgr), _world(world), _laps(laps),_checksPerLap(checksPerLap), 
                           _canalesSonido(canalesSonido), _groundObject(groundObject), _id(id)
{
        // Creamos una instancia de CarRayCast
        _car = unique_ptr<CarRayCast>(new CarRayCast(_nombreVehiculo,_posicionSalida,_sceneMgr,_world,nullptr,_id));
        
        
    
        if (buildForMe)
            build();
}

humanPlayer::~humanPlayer()
{
}

void humanPlayer::build()
{
    _car->buildVehiculo();

    // Por defecto el coche mira de frente a la cámara. Orientamos el coche donde nos interese, en este caso que mire a la izquierda (-X)
    btTransform trans = _car->getRigidBody()->getBulletRigidBody()->getCenterOfMassTransform();
    btQuaternion quat;
    quat.setEuler(Radian(Ogre::Degree(-90)).valueRadians(),0,0);
    trans.setRotation(quat);
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
    

    // Todos estos estados tiene sentido que inicialicen una se ha llamado a CarRayCast::buildVehiculo();
    _idCheck_destino = 0;
    _idCheck_origen = _checksPerLap-1;
    _totalCheckPoints = 0;
    _idCheck_meta =  (_checksPerLap-1) * _laps;
    _finish = false;
    _sentidoContrario = false;
    _onHisWay = false;
    _timeWrongWay = 0;
    _marchaActual = 0; // Marcha actual = punto muerto :D
    _incrementoMarcha = _car->getFuerzaMotor() / MAX_MARCHA; // Incremento de velocidad al subir de marcha
    _timeMarcha = 0;   // Acumulador de tiempo para controlar cuando cambiamos de marcha;
    _aceleracion = 0;
    _timeVolandoVolcado = 0;
    _nodoCheckPointSiguiente = nullptr;
    _nodoCheckPointAnterior = nullptr;
    _starting = true;
    _atajando = false;
    _fueraPista = false;
    _timeFueraPista = 0;
    _posicionY = _car->getPosicionActual().y;
    _lapActual = 0;
    
    // Para cuando lancemos rayos hacia delante si el resultado es igual a this no nos vale.
//    static_cast<rigidBody_data*>(_car->getRigidBody()->getBulletObject()->getUserPointer())->_data = this;
    
    _ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);

}

void humanPlayer::update(Real deltaT, size_t keys)
{
    _deltaT = deltaT;
    
    if (_onHisWay && !_finish) // En ruta, go go go go !!!!
    {
        if (keys & static_cast<size_t>(keyPressed_flags::RECOLOCAR))
        {
            cout << "reseteo manual" << endl;
            recoloca();
            
        }
        if (keys & static_cast<size_t>(keyPressed_flags::UP))
        {
            if (sounds::getInstance()->isMixPlaying(_canalesSonido.MOTORDOWN.id)) sounds::getInstance()->halt_effect(_canalesSonido.MOTORDOWN.id);
            if (!sounds::getInstance()->isMixPlaying(_canalesSonido.MOTORUP.id)) 
            { 
                if (_car->getVelocidadKmH() > 10)
                {
                   sounds::getInstance()->play_effect_loop("motorRunning",_canalesSonido.MOTORUP.id); 
                   sounds::getInstance()->setVolume("motorRunning",_canalesSonido.MOTORUP.volumen);
                }
                else
                {
                    sounds::getInstance()->play_effect("motor1up",_canalesSonido.MOTORUP.id);
                    sounds::getInstance()->setVolume("motor1up",_canalesSonido.MOTORUP.volumen);
                }
            }
                
            cambiaMarcha();
            if ((keys & static_cast<size_t>(keyPressed_flags::LEFT)) || (keys & static_cast<size_t>(keyPressed_flags::RIGHT)))
               _car->acelerar(_aceleracion,false);
            else 
                _car->acelerar(_aceleracion,true);
            
        }
        else
        {
            if (sounds::getInstance()->isMixPlaying(_canalesSonido.MOTORUP.id)) sounds::getInstance()->halt_effect(_canalesSonido.MOTORUP.id);
            if (!sounds::getInstance()->isMixPlaying(_canalesSonido.MOTORDOWN.id) && _car->getVelocidadKmH() > 0)
            {
                //MOTOR1DOWN
                sounds::getInstance()->play_effect("motor1down",_canalesSonido.MOTORDOWN.id);
                sounds::getInstance()->setVolume("motor1down",_canalesSonido.MOTORDOWN.volumen);
            }
            _car->acelerar(0); // Si no aceleramos que actúe la inercia. También sirve para cuando soltamos el freno :D
            if (_marchaActual && _car->getVelocidadKmH() < 1) _marchaActual = 0;
        }
        
        if (keys & static_cast<size_t>(keyPressed_flags::DOWN)) //_car->frenar();
        {
            if ((keys & static_cast<size_t>(keyPressed_flags::LEFT)) || (keys & static_cast<size_t>(keyPressed_flags::RIGHT)))
                _car->frenar(false);
            else
                _car->frenar(true);
            
        }
        else if (!(keys & static_cast<size_t>(keyPressed_flags::DOWN)) && !(keys & static_cast<size_t>(keyPressed_flags::UP)))
            _car->acelerar(0);
        
        
        if (keys & static_cast<size_t>(keyPressed_flags::LEFT)) _car->girar(1);
        else if (!(keys & static_cast<size_t>(keyPressed_flags::RIGHT))) _car->enderezar();
        if (keys & static_cast<size_t>(keyPressed_flags::RIGHT)) _car->girar(-1);
        else if (!(keys & static_cast<size_t>(keyPressed_flags::LEFT))) _car->enderezar();
    
        compruebaCheckPoint();
        compruebaSuelo();
        compruebaRecolocar();
        compruebaChirriar();
        
        if (_finish) _car->acelerar(0);
            
    }
}


void humanPlayer::stop()
{
    //_onHisWay = false; 
    _car->acelerar(0); 
    _car->stop(); 
}

bool humanPlayer::compruebaCheckPoint()
{
    //https://youtu.be/nyJa-WKmWqE GRACIAS A DIOS ENCONTRÉ A ESTE CRACK. Documentación Bullet = BigShit!
    Vector3 inicio =  _car->getPosicionActual();
    Vector3 fin = inicio;
    inicio.y += -0.1;
    fin.y += -2;  // Alargar el rayo lo suficiente para alcanzar los checkpoints que hay por debajo del circuito.
    
//    dibujaLinea(inicio,fin);
    btCollisionWorld::AllHitsRayResultCallback rayCallback(convert(inicio),convert(fin));
                                                           
    _world->getBulletDynamicsWorld()->rayTest(convert(inicio),convert(fin), rayCallback);
    
    if (rayCallback.hasHit())
    {
        tipoRigidBody tAux = tipoRigidBody::UNKNOWN;
        for (int i=0; i<rayCallback.m_collisionObjects.size(); i++)
        {
            if (rayCallback.m_collisionObjects[i]->getUserPointer())
            {
                tipoRigidBody t = static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_tipo;
                if (t == tipoRigidBody::CHECK_POINT)
                {   
                    size_t id = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_id;
                        
                    if (_idCheck_destino == id && (_idCheck_origen < id || _starting))      // Si el idcheck_destino es igual al checkpoint por el que estoy pasando, lo hemos alcanzado.
                    {                                                         // Y para no entrar aquí más veces de las necesarias comprobamos que el origen sea distinto del 
                        _idCheck_origen = _idCheck_destino;                   // checkPoint por el que paso

                        if (_totalCheckPoints == _idCheck_meta && !_finish)
                        {
                            _finish = true;
                            cout << "******************************************************************************************************************************" << endl;
                            cout << "*********************************          *****    *****     ******   *******************************************************" << endl;
                            cout << "*********************************          *****    *****   *  *****   *******************************************************" << endl;
                            cout << "*********************************   ************    *****   **  ****   *******************************************************" << endl;
                            cout << "*********************************       ********    *****   ***  ***   *******************************************************" << endl;
                            cout << "*********************************       ********    *****   ****  **   *******************************************************" << endl;
                            cout << "*********************************   ************    *****   *****      *******************************************************" << endl;
                            cout << "*********************************   ************    *****   ******     *******************************************************" << endl;
                            cout << "******************************************************************************************************************************" << endl;
                            break;
                        }

                        _idCheck_destino++;
                        _idCheck_destino = _idCheck_destino % _checksPerLap;
                        
                        if (_starting ) _starting = false;
                        if (id == _checksPerLap - 1) _starting = true;
                        
                        if (id == 0)
                            _lapActual++;
                            
                        _totalCheckPoints = (id+1) + (_lapActual-1) * (_checksPerLap-1);
                            
                        //_totalCheckPoints++;
                        
                        _nodoCheckPointAnterior = _nodoCheckPointSiguiente;
                        _nodoCheckPointSiguiente = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_ogreNode;
                        
//                        cout << "Destino alcanzado. Nuevo destino: " << _idCheck_destino << endl;
//                        cout << "Origen actualizado a: " << _idCheck_origen << endl;
//                        cout << "Total checkpoints: " << _totalCheckPoints << endl;
                        _timeWrongWay = 0;
                        _sentidoContrario = false;
                        _atajando = false;
                        _fueraPista = false;
                        _timeAtajando = 0;
                        _ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
                    }
                    else if (id < _idCheck_origen && (!_starting || id != _checksPerLap -1))    // Si el id del checkpoint por el que estoy pasando es mayor que el de destino, es que hemos avanzado hacia atrás: AVISAR Y PENALIZAR.
                    {       
//                            cout << "Yendo hacia atrás. Checkpoint de destino era: " << _idCheck_destino << " Y el checkpoint por el que paso es " << id << endl;


//                            _descuentoCheckpoints += 1;
//                            cout << "Checkpoints de descuento: " << _descuentoCheckpoints << endl;
//                        
//                            _idCheck_origen =  id;          // Ahora el origen será el id actual (aunque realmente ya lo hubiéramos pasado)
//                            _nodoCheckPointAnterior = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_ogreNode;                                                                       
//                            _nodoCheckPointSiguiente = _nodoCheckPointAnterior;
//                            _sentidoContrario = true;
//                            _idCheck_destino = id + 1;
                            
                            
//                            cout << "Ahora el check destino es: " << _idCheck_destino << endl;
//                            cout << "Y checkpoins totales: " << _totalCheckPoints << endl;
                    }
                    else if ((static_cast<int>(id) - static_cast<int>(_idCheck_origen)) > 1 && _nodoCheckPointSiguiente)
                    {
                        cout << "atajando" << endl;
                        cout << "Se ha saltado " <<((int)id - (int)_idCheck_origen) << " checkpoins " << endl;
                        cout << "Checkpoint sobre el que estoy: " << id << endl;
                        cout << "Checkpoint origen: " << _idCheck_origen << endl;
                        cout << "Checkpoint destino: " << _idCheck_destino << endl;
                        cout << "Nodo anterior: " << _nodoCheckPointAnterior << endl;
                        cout << "Nodo siguiente: " << _nodoCheckPointSiguiente << endl;
                        _atajando = true;
                    }
                }

                // Faltaría hacer pruebas más exhaustivas pero parece que a veces el orden de los objetos impactados por el rayo no es el esperado.
                // El rayo se lanza hacia abajo y puede colisionar con el circuito, la carretera, un checkpoint o incluso otro coche (si por azar se coloca encima).
                // Como la carretera es un plano que está justo debajo del circuito (en realidad debería llamarse escenario) cuando el coche se sale de la pista
                // el rayo impacta con el plano que está debajo que hace de carretera y también con el circuito por lo que no basta con comprobar si impacta con la
                // carretera pues daría un positivo aún estando fuera de la carretera. Lo que si es seguro, es que si está en la carretera el rayo no debería impactar
                // con el circuito pues por donde el coche circula solo toca el plano de la carretera, de modo que hacemos esta ñapa y parece que va bien.
                // Aunque lo ideal sería comprobar el correcto funcionamiento del callback de rayos y trastear con rayCallback.m_closestHitFraction donde seguramente
                // el valor de ese miembro permita ordenar los objetos impactados por el rayo.
                if (tAux != tipoRigidBody::CIRCUITO && t == tipoRigidBody::CIRCUITO)
                    tAux = tipoRigidBody::CIRCUITO;
                    
            }
        }
        
        if (tAux == tipoRigidBody::CIRCUITO)
            _fueraPista = true;
        else _fueraPista = false;
            
        return true;
    }
    
    return false;

}

void humanPlayer::dibujaLinea(Vector3 inicio, Vector3 fin, bool consoleOut)
{
        // DIBUJAMOS LINEA A MODO DE FARO PARA SABER A DONDE HA DE DIRIGIRSE EL COCHE
        static int i = 1000;
        ManualObject* manual = _sceneMgr->createManualObject("rayo_" + to_string(i));
        manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
            manual->position(inicio); //start
            manual->position(fin);    //end
        manual->end();
        i++;
        _sceneMgr->getRootSceneNode()->attachObject(manual);    
        
        if (consoleOut)
            cout << this->getNombreEnPantalla() << " lanzando rayo desde " << inicio << " hasta " << fin << endl;

}

void humanPlayer::cambiaMarcha()
{
    if (_marchaActual == 0)                 // Arrancamos. En este bloque if no entraremos más hasta que en otro momento marchaActual valga 0
    {                                       // Usualmente se pondrá a cero al recolocar el coche después de un accidente, salida de pista, etc.
        _timeMarcha = 0;                    // Reseteamos tiempo para empezar a contar tiempo hasta el siguiente cambio de marcha.
        _aceleracion = 0;                   // Reseteamos aceleracion. Estamos arrancando.
        _marchaActual ++;                   // Ponemos primera.
        _aceleracion = (_car->getFuerzaMotor() * ((Ogre::Real)_marchaActual/MAX_MARCHA)) * _car->getPotenciadorPrimera();  // Incrementamos aceleración.
        cout << "primera, potenciador: " << _car->getPotenciadorPrimera() << "\taceleracion: " << _aceleracion << endl;

    }
    else if (_timeMarcha > MAX_TIME_MARCHA_HUMAN)     // Si entramos aquí es que por lo menos íbamos en primera y el tiempo ha alcanzado el máximo 
    {                                           // establecido para cambiar de marcha.
        if (_marchaActual < MAX_MARCHA)        // Si no hemos llegado al tope de marchas.
        {
            _marchaActual++;
            _aceleracion = _car->getFuerzaMotor() * ((Ogre::Real)_marchaActual/MAX_MARCHA);  // Incrementamos aceleración.
        }
        else if (_marchaActual == MAX_MARCHA)
        {
            _aceleracion = _car->getFuerzaMotor() * 1.1;
        }
        
        _timeMarcha = 0;

    }
    
    _timeMarcha += _deltaT;

}

void humanPlayer::recoloca()
{
    Vector3 aux;
    if (_nodoCheckPointAnterior)
    {
        aux = _nodoCheckPointAnterior->getPosition();
        aux.y = _posicionY;
    }
    else // Posicion de salida
        aux = _posicionSalida;
        
    _car->recolocar(aux,convert(_ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA    
    _timeWrongWay = 0;
    _timeAtajando = 0;
    _timeFueraPista = 0;
    _timeVolandoVolcado = 0;
    _sentidoContrario = false;
    _atajando = false;
    _fueraPista = false;
    stop();
    
    cout << "******************************************************************************************************************************" << endl;
    
}

void humanPlayer::compruebaRecolocar()
{
    if (_sentidoContrario)
        _timeWrongWay += _deltaT;
        
    if (_atajando)
        _timeAtajando += _deltaT;

    if ((_sentidoContrario && _timeWrongWay >= MAX_TIME_WRONGWAY) || (_atajando && _timeAtajando >= MAX_TIME_WRONGWAY) ||
        (_fueraPista && _timeFueraPista >= MAX_TIME_WRONGWAY) || (_timeVolandoVolcado >= MAX_TIME_WRONGWAY))
    {
        cout << "CompruebaRecolocar:" << endl;
        cout << "\t SentidoContrario: " << _sentidoContrario << endl;
        cout << "\t Atajando: " << _atajando << endl;
        cout << "\t FueraPista: " << _fueraPista << endl;
        cout << "Checkpoins totales: " << _totalCheckPoints << endl;


        recoloca();
      
    }
    
} 

void humanPlayer::compruebaSuelo()
{
   if (!_car->ruedasEnContacto())
   {
      cout << "*******************************************************************************************************************************" << endl;
      cout << "O HA VOLCADO O VA VOLANDO BAJITO, CUIDADO CON LA CONFIGURACIÓN DE LOS PARÁMETROS DEL COCHE ************************************" << endl;
      cout << "*******************************************************************************************************************************" << endl;
      _timeVolandoVolcado += _deltaT;
   }
   else
      _timeVolandoVolcado = 0;

   if (_fueraPista)
      _timeFueraPista += _deltaT;
   else
      _timeFueraPista = 0;
}

void humanPlayer::coutTipoCollisionObject(tipoRigidBody t)
{
    string aux;
    switch (t)
    {
        case tipoRigidBody::CARRETERA: aux = "CARRETERA";   
                                        break;
        case tipoRigidBody::CHECK_POINT: aux = "CHECKPOINT"; 
                                            break;
        case tipoRigidBody::CIRCUITO: aux = "CIRCUITO"; 
                                        break;
        case tipoRigidBody::COCHE: aux = "COCHE"; 
                                            break;
        case tipoRigidBody::OBSTACULO: aux = "OBSTACULO"; 
                                        break;
        default: aux = "NI IDEA!";
    }
    
    cout << "tipo collisionobject: " << aux << endl;
}

void humanPlayer::compruebaChirriar()
{
    skidValues skiding = _car->getSkidding();
    _car->getRuedasChirriando(_valoresSkidRuedas);
    
    if (_valoresSkidRuedas.size())
    {
        if (!sounds::getInstance()->isMixPlaying(_canalesSonido.SKIDING.id))
        {    
            for(size_t j=0; j<_valoresSkidRuedas.size();j++) cout << "skid rueda " << j << ":" << _valoresSkidRuedas.at(j) << endl;
            Ogre::Real skid = _valoresSkidRuedas.at(0);
            //if (skid <= 0.1 && skid > 0.075)           // chirrío corto
            if (skid <= skiding.maxSkidValue && skid > skiding.SkidCorto)           // chirrío corto
            {
                cout << "chirrío corto " << endl;
                sounds::getInstance()->play_effect("skiddingCorto",_canalesSonido.SKIDING.id);
                sounds::getInstance()->setVolume("skiddingCorto",sounds::getInstance()->maxVolume()*0.25);
            }
            //else if (skid <= 0.075 && skid > 0.025)     // chirrío medio
            else if (skid <= skiding.SkidCorto && skid > skiding.SkidMedio)     // chirrío medio
            {
                cout << "chirrío medio " << endl;
                sounds::getInstance()->play_effect("skiddingMedio",_canalesSonido.SKIDING.id);
                sounds::getInstance()->setVolume("skiddingMedio",sounds::getInstance()->maxVolume()*0.25);
            }
            //else if (skid <= 0.01 && skid > minimo)                   // chirrío largo
            else if (skid <= skiding.SkidMedio && skid > skiding.SkidLargo)                   // chirrío largo
            {
                cout << "chirrío largo: " << skid << endl;
                sounds::getInstance()->play_effect("skiddingLargo",_canalesSonido.SKIDING.id);
                sounds::getInstance()->setVolume("skiddingLargo",sounds::getInstance()->maxVolume()*0.25);
            }   
        }
    }
}
