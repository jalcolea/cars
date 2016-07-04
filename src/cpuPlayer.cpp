#include "cpuPlayer.h"
#include "iamanager.h"
#include "CarRayCast.h"
//#include "bulletUtil.h"
#include "puntoManager.h"
#include "sounds.h"

using namespace Ogre;

cpuPlayer::cpuPlayer(string nombreEnPantalla, string nombreVehiculo, string nombreMaterial, string ficheroRutasIA, Vector3 posicionSalida, 
                     SceneManager* sceneMgr, DynamicsWorld* world, size_t laps, void* groundObject, size_t id)
         : _nombreEnPantalla(nombreEnPantalla), _nombreVehiculo(nombreVehiculo), _nombreMaterial(nombreMaterial), _ficheroRutasIA(ficheroRutasIA), _posicionSalida(posicionSalida),
           _sceneMgr(sceneMgr), _world(world), _laps(laps), _groundObject(groundObject), _id(id)
{
        // Creamos una instancia de CarRayCast
        _car = unique_ptr<CarRayCast>(new CarRayCast(_nombreVehiculo,_posicionSalida,_sceneMgr,_world,nullptr,_id));
        
        // Obtenemos lista de CheckPoints para la trazada
        _iapd = unique_ptr<IAPointsDeserializer>(new IAPointsDeserializer());
        _iapd->cargarFichero(_ficheroRutasIA);
        // Instanciamos un puntoManager para gestionar la trazada
        _iaMgr = unique_ptr<puntoManager>(new puntoManager());
        
        // Le damos los puntos al puntoManager para gestionarlos
        auto puntos = _iapd->getPoints();

        for (size_t i=0; i<puntos.size(); i++)
        {
            _iaMgr->addPunto(puntos.at(i));
//            if (i>0) dibujaLinea(puntos[i-1].p,puntos[i].p);
//            dibujaLinea(puntos[i].p,Vector3(puntos[i].p.x,puntos[i].p.y+5,puntos[i].p.z));

        }

        // Le pedimos al puntoManager que nos calcule puntos aleatorios para cada Checkpoint dado.
        // Cada instancia de esta clase manejará su lista de puntos aleatorios para cada Checkpoint
        // de modo que cada coche tendrá una lista ligeramente distinta y por ende una trazada también distinta.
        // Se pueden calcular varios puntos aleatorios para un mismo Checkpoint. Útil si se usa puntoManager
        // como un Singleton, de este modo cada coche podría elegir un punto aleatorio aún compartiendo la misma instancia
        // de puntoManager.
        _iaMgr->derivaPuntos(7,                   // Ancho de un checkpoint, le pasamos un ancho menor del real para que no de puntos muy cercanos a los bordes de la pista.
                             3,                   // Cuantos puntos aleatorios calcular. Para darle mayor aleatoriedad.
                             false,                // Distribuir los puntos en tantos rangos como puntos a calcular.
                             Vector3::UNIT_X,     // Sobre que eje estamos calculando los puntos aleatorios
                             1);                // Margen entre los rangos en los que se calculan los puntos aleatorios. 
                             
//        _iaMgr->derivaPuntos(7,                   // Ancho de un checkpoint, le pasamos un ancho menor del real para que no de puntos muy cercanos a los bordes de la pista.
//                             3,                   // Cuantos puntos aleatorios calcular. Para darle mayor aleatoriedad.
//                             true,                // Distribuir los puntos en tantos rangos como puntos a calcular.
//                             Vector3::UNIT_X,     // Sobre que eje estamos calculando los puntos aleatorios
//                             2);                // Margen entre los rangos en los que se calculan los puntos aleatorios. 
                             
        
//        for (size_t i=0; i < _iaMgr->getPuntos().size(); i++)
//        {
//            for (size_t j=0; j<_iaMgr->getPunto(i).derivados.size(); j++)
//            {
//                Vector3 auxFin = _sceneMgr->getSceneNode("CheckPointPlane_"+to_string(i))->convertLocalToWorldPosition(_iaMgr->getPunto(i).derivados[j]);
//                Vector3 auxInicio = auxFin;
//                auxFin.y +=10;
//                dibujaLinea(auxInicio,auxFin);
//            }
//        }
        
        
        
}


cpuPlayer::~cpuPlayer()
{
}

void cpuPlayer::build()
{
    _car->buildVehiculo();

    // Por defecto el coche mira de frente a la cámara. Orientamos el coche donde nos interese, en este caso que mire a la izquierda (-X)
    btTransform trans = _car->getRigidBody()->getBulletRigidBody()->getCenterOfMassTransform();
    btQuaternion quat;
    quat.setEuler(Radian(Ogre::Degree(-90)).valueRadians(),0,0);
    trans.setRotation(quat);
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
    

    // Todos estos estados tiene sentido que inicialicen una vez se ha llamado a CarRayCast::buildVehiculo();
    _idCheck_destino = 0;
    //_idCheck_origen = 0;
    _idCheck_origen = _iaMgr->getPuntos().size() - 1;
    _totalCheckPoints = 0;
    _idCheck_meta = (_iaMgr->getPuntos().size()  * _laps);
    _finish = false;
    _sentidoContrario = false;
    _onHisWay = false;
    _timeStopped = 0;
    _timeWrongWay = 0;
    _maniobra = estadoManiobra::ENRUTA;
    _marchaActual = 0; // Marcha actual = primera :D
    _incrementoMarcha = _car->getFuerzaMotor() / MAX_MARCHA; // Incremento de velocidad al subir de marcha
    _timeMarcha = 0;   // Acumulador de tiempo para controlar cuando cambiamos de marcha;
    _timeHandicap = 0; // Acumulador de tiempo para aplicar el handicap aleatorio.
    _aceleracion = 0;
    _puntosAleatoriosPasados = 0; // Cuando alternemos la ruta para pasar por puntos aleatorios de los checkPoints, cada N puntos alcanzados
                                  // retomaremos la ruta principal.
    _rutaPuntosAleatorios = true; // La ponemos a true desde un principio para que en la salida cada coche intente ir hacia un punto aleatorio.
    
    _nodoCheckPointSiguiente = nullptr;
    
    // Para cuando lancemos rayos hacia delante si el resultado es igual a this no nos vale.
//    static_cast<rigidBody_data*>(_car->getRigidBody()->getBulletObject()->getUserPointer())->_data = this;
    
    setListaPuntosAleatorios();
}

void cpuPlayer::update(Real deltaT)
{
    if (!_finish && _onHisWay)
    {
        Vector3 origen;
        Vector3 destino;
//        static Vector3 destinoOld = Vector3::ZERO; // Inicializar a Zero o a una posicion que no tenga sentido en el juego
        Vector3 direccion = Vector3::ZERO;
        Vector3 direccionActualCoche;
        btTransform btTrans;
        Real distancia;
        Quaternion quat;
        
        _deltaT = deltaT;
        
        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        compruebaCheckPoint();
        
        destino = obtenerDestino();
        
        compruebaManiobra();
        
        compruebaRecolocar();
        
        cambiaMarcha();

        aplicarHandicap();
        
        
/*        if (destinoOld != destino)
//        {
//            destinoOld = destino;
//            _onHisWay = false;
//            //origen.y = destino.y;
//            destino.y = origen.y;
//            direccion = destino - origen;
//            direccion.normalise();
//            //dibujaLinea(origen,destino);
//            //dibujaLinea(destino,Vector3(destino.x,destino.y+10,destino.z));
//        }
*/        
        direccionActualCoche = convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());

        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        //origen.y = direccionActualCoche.y;
        origen.y = destino.y;
        direccion = destino - origen ;

        Real angulo = direccionActualCoche.getRotationTo(direccion,Vector3::UNIT_Y).getYaw().valueRadians();
//        cout << "angulo entre ellos = " << angulo << endl; 
        if (abs(angulo) > 0.04) // PRIMERO COMPROBAMOS QUE AL ANGULO ENTRE ELLOS ES TODAVÍA SUFICIENTEMENTE GRANDE PARA SEGUIR GIRANDO (0.02 radianes = 1º)
        {
            if (abs(angulo) > Ogre::Math::Math::HALF_PI && abs(angulo) < Ogre::Math::PI && _car->getVelocidadKmH() < 5)// || _maniobra == estadoManiobra::BORDILLO_CERCA)
            {    
//                dibujaLinea(origen,Vector3(origen.x,origen.y+5,origen.z));
//                dibujaLinea(destino,Vector3(destino.x,destino.y+10,destino.z));
//                cout << "angulo mu gordo " << angulo << endl;
//                cout << "SceneNode checkpoint destino actual: " << (_nodoCheckPointSiguiente?_nodoCheckPointSiguiente->getName():"UNKNOWN") << endl;
//                cout << "Posicion Origen Actual: " << origen << endl;
//                cout << "Posicion destino Actual: "  << destino << endl;
//                cout << "Direccion resultante (destino-origen): " << direccion << endl;
//                cout << "Checkpoint del que vengo: " << _idCheck_origen << endl;
//                cout << "Checkpoint al que tengo que llegar: " << _idCheck_destino << endl;
                
                _maniobra = estadoManiobra::MANIOBRANDO_MARCHA_ATRAS;
                
            }
            else if (_maniobra == estadoManiobra::MANIOBRANDO_MARCHA_ATRAS && angulo <= MAX_VALOR_GIRO_RUEDAS)
                _maniobra = estadoManiobra::ENRUTA; 

            if (_maniobra == estadoManiobra::MANIOBRANDO_MARCHA_ATRAS)
            {
                _aceleracion = abs(_aceleracion) * -1;
                angulo *= -1;
            }    
            
                _car->girarCPU( angulo);
        }
        else
        {
            _car->girarCPU(0);
            angulo = 0;
        }
        
        //_car->acelerarCPU(_car->getFuerzaMotor(),false);
        _car->acelerarCPU(_aceleracion,false);
    }
    else _car->acelerarCPU(0,false);

}


bool cpuPlayer::compruebaCheckPoint()
{
    //https://youtu.be/nyJa-WKmWqE GRACIAS A DIOS ENCONTRÉ A ESTE CRACK. Documentación Bullet = BigShit!
    Vector3 inicio =  _car->getPosicionActual();
    Vector3 fin = inicio;
    fin.y += -5;  // Alargar el rayo lo suficiente para alcanzar los checkpoints que hay por debajo del circuito.
    
    //dibujaLinea(inicio,fin);
    btCollisionWorld::AllHitsRayResultCallback rayCallback(convert(inicio),convert(fin));
                                                           
    _world->getBulletDynamicsWorld()->rayTest(convert(inicio),convert(fin), rayCallback);
    
    if (rayCallback.hasHit())
    {
        for (int i=0; i<rayCallback.m_collisionObjects.size(); i++)
        {
            if (rayCallback.m_collisionObjects[i]->getUserPointer())
            {
                //coutTipoCollisionObject(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_tipo);
                if (static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_tipo == tipoRigidBody::CHECK_POINT)
                {   
                    size_t id = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_id;
//                    cout << _nodoCheckPointSiguiente << endl;
//                    if (_nodoCheckPointSiguiente) 
//                        cout << "Nombre del sceneNode del siguiente Checkpoint" << _nodoCheckPointSiguiente->getName() << endl;
                        
/*                    if (_idCheck_destino < id) // Si el id del checkpoint por el que estoy pasando es mayor que el de destino, es que hemos avanzado hacia atrás: AVISAR Y PENALIZAR.
                    {
                        //Pasando por meta en salida.
                        _sentidoContrario = true;
                    }
                    else*/ if (_idCheck_destino == id && _idCheck_origen != id) // Si el idcheck_destino es igual al checkpoint por el que estoy pasando, lo hemos alcanzado.
                    {                                                         // Y para no entrar aquí más veces de las necesarias comprobamos que el origen sea distinto del 
                                                                              // checkPoint por el que paso
                        _idCheck_origen = _idCheck_destino; 
                        if (_totalCheckPoints == _idCheck_meta && !_finish)
                        {
                            _finish = true;
                            //stop();
                        }
                        _idCheck_destino++;
                        _idCheck_destino = _idCheck_destino % _iaMgr->getPuntos().size();

/* DE LOCOS */          _nodoCheckPointSiguiente = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_ogreNode;

                        
/* PROBANDO */          _puntosAleatoriosPasados++;
/* PROBANDO */          if (_puntosAleatoriosPasados == 3)
/* PROBANDO */          { 
/* PROBANDO */              _idxPuntoAleatorioActual %= _iaMgr->getPunto(_idCheck_destino).derivados.size();
/* PROBANDO */              _rutaPuntosAleatorios = false;
/* PROBANDO */          }
                        
                        _totalCheckPoints++;
                    }
                    
                    break;
                }
            }
        }
        return true;
    }
    
    return false;
}



void cpuPlayer::dibujaLinea(Vector3 inicio, Vector3 fin, bool consoleOut)
{
    // DIBUJAMOS LINEA A MODO DE FARO PARA SABER A DONDE HA DE DIRIGIRSE EL COCHE
        static int i = 0;
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

void cpuPlayer::setListaPuntosAleatorios()
{
    // Establecemos que lista de puntos aleatorios vamos a manejar desde un principio.
    // La cambiaremos cuando la distancia entre un checkPoint y el siguiente sea lo suficientemente
    // grande. Si fuéramos cambiando a cada CheckPoint que alcancemos y los Checkpoints estuvieran
    // muy cerca uno de otro, el coche podría empezar a zigzaguear sin sentido.
    // rand() no es que sea una maravilla dando números aleatorios así que ampliamos el rango y normalizamos
    // los resultados al rango real.
    srand(time(nullptr));
    int rango = 10;
    int r = rand() % (_iaMgr->getPunto(_idCheck_destino).derivados.size() * rango);
    r = r % _iaMgr->getPunto(_idCheck_destino).derivados.size();
    
    _idxPuntoAleatorioActual = r;
//    cout << "*********************************************************************************************************** " << _idxPuntoAleatorioActual << endl;
}

Ogre::Vector3 cpuPlayer::getPuntoAleatorioEnWS() // EnWS = En World Space
{
    //Vector3 aux = _iaMgr->getPuntoDerivado(_idCheck_destino,_idxPuntoAleatorioActual);
    Vector3 aux = _iaMgr->getPunto(_idCheck_destino).derivados[_idxPuntoAleatorioActual];
//    cout << "punto aleatorio a convertir " << aux << endl;
    //aux = _nodoCheckPointSiguiente->convertLocalToWorldPosition(aux);
    aux = _iaMgr->conviertePuntoAWS(_nodoCheckPointSiguiente,aux,Vector3(0,0,1));
//    cout << "punto aleatorio obtenido " << aux << endl;
    return aux; 
}

Ogre::Vector3 cpuPlayer::obtenerDestino()
{
    if (!_nodoCheckPointSiguiente) // Si no conocemos el puntero al SceneNode del checkpoint siguiente, estamos pasando por meta hacia salida.
    {
        Vector3 aux = _iaMgr->getPunto(_idCheck_destino).p;
        aux.z = _car->getPosicionActual().z;
        return aux; // Entonces simplemente nos dirigimos hacia la izquierda sobre el eje X pero respetando la Z del coche, para que vaya recto hacia la izquierda
                    // Así evitamos apelotonamientos en la salida. Aunque en cuanto tengan un destino marcado intentarán alinearse. Es la guerra!!!!
        //return (_iaMgr->getPunto(_idCheck_destino)).p;
    }
    else
        if (_maniobra == estadoManiobra::COCHE_DELANTE_CERCA)
        {
            _maniobra = estadoManiobra::ENRUTA;
            _rutaPuntosAleatorios = true; // Si tenemos un coche delante cerca empezamos a usar puntos aleatorios.
            _puntosAleatoriosPasados = 0; // Ponemos a cero el contador de puntos aleatorios alcanzados.
            return getPuntoAleatorioEnWS(); // Sacar un punto aleatorio y convertirlo a coordenadas Globales 
        }
        else
        {
            if (!_rutaPuntosAleatorios)
            {
                //_maniobra = estadoManiobra::ENRUTA;
                return _iaMgr->trasladaAlBorde(_nodoCheckPointSiguiente,_iaMgr->getPunto(_idCheck_destino).p,Vector3(0,0,1));
            
            }
            else
            {
                return getPuntoAleatorioEnWS(); // Sacar un punto aleatorio y convertirlo a coordenadas Globales 
            }
        }

            
}

void cpuPlayer::compruebaRecolocar()
{
     if (!_car->ruedasEnContacto())
    {
        if (_timeStopped > MAX_TIME_STOPPED)
        {
            //Vector3 aux = _iaMgr->getPunto(_idCheck_destino-1).p; aux.y += 0.1;// AUMENTAR LA Y PARA QUE EL COCHE NO PUEDA QUEDARSE ESTANCADO.
            Vector3 aux = _iaMgr->getPunto((_idCheck_destino)?(_idCheck_destino-1):_iaMgr->getPuntos().size()-1).p; aux.y += 0.25;
            //Vector3 aux = _iaMgr->getPunto(_totalCheckPoints % _iaMgr->getPuntos().size()).p;
//            cout << "RUEDAS NO EN CONTACTO: \n " << aux << "\n" << _totalCheckPoints % _iaMgr->getPuntos().size() << endl;
            _car->recolocar(aux,convert(_ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
            _maniobra = estadoManiobra::ENRUTA;
        }
        if (_marchaActual) _marchaActual = 0;
    }                                                           
    else
    {
        _ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
    }
    
    if (_car->getVelocidadKmH() >= 2)
    {
        _timeStopped = 0;
        _ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
    }
    else
    {
        if (_timeStopped > MAX_TIME_STOPPED)
        {
            //Vector3 aux = (_iaMgr->getPunto(_idCheck_destino-1)).p; aux.y += 0.1;
            Vector3 aux = _iaMgr->getPunto((_idCheck_destino)?(_idCheck_destino-1):_iaMgr->getPuntos().size()-1).p; aux.y += 0.25;
            //Vector3 aux = _iaMgr->getPunto(_totalCheckPoints % _iaMgr->getPuntos().size()).p;
            cout << "PARADO MUCHO TIEMPO: \n " << aux << "\n" << _totalCheckPoints % _iaMgr->getPuntos().size() << endl;
            _car->recolocar(aux,convert(_ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
            _maniobra = estadoManiobra::ENRUTA;
            if (_marchaActual) _marchaActual = 0;
        }
        else
        {
            _timeStopped += _deltaT;
        }
    }
   
}

void cpuPlayer::compruebaManiobra()
{
    rayoAlFrente();
}

void cpuPlayer::rayoAlFrente()
{
    Vector3 forward = _car->getSceneNode()->getOrientation() * Vector3(0,0,1);
    //Vector3 inicio = _car->getPosicionActual();
    //Vector3 inicio = _car->getSceneNode()->convertLocalToWorldPosition(Vector3(0,0,1.5));
    // La posicion origen desde la que se lanza el rayo la colocamos en el borde de las dimensiones de un coche.
    Vector3 inicio = _car->getSceneNode()->convertLocalToWorldPosition(Vector3(0,0,_car->getSceneNode()->getAttachedObject(0)->getBoundingRadius()+0.3));
    Vector3 fin = inicio + forward;

//    Vector3 backward = node->getOrientation() * Vector3(0,0,1);
//    Vector3 left= node->getOrientation() * Vector3(-1,0,0);
//    Vector3 right = node->getOrientation() * Vector3(1,0,0);
//    Vector3 up = node->getOrientation() * Vector3(0,1,0);
//    Vector3 down = node->getOrientation() * Vector3(0,-1,0);

//    dibujaLinea(inicio,fin,true);
    btCollisionWorld::ClosestRayResultCallback rayCallback(convert(inicio),convert(fin));
                                                           
    _world->getBulletDynamicsWorld()->rayTest(convert(inicio),convert(fin), rayCallback);
    
    if (rayCallback.hasHit())
    {
        
        if (rayCallback.m_collisionObject->getUserPointer())
        {
            tipoRigidBody t = static_cast<rigidBody_data*>(rayCallback.m_collisionObject->getUserPointer())->_tipo;
            //cout << _nombreEnPantalla << " (" << _nombreMaterial << ") lanza rayo e impacta con "; coutTipoCollisionObject(t);
            switch(t)
            {
                case tipoRigidBody::COCHE:          //cout << "_closestHitFraction "<< rayCallback.m_closestHitFraction << endl;
                                                    if (rayCallback.m_closestHitFraction <= 0.5 && rayCallback.m_closestHitFraction > 0)
                                                    {
                                                        //if (_car->getRigidBody()->getBulletObject() == rayCallback.m_collisionObject)
                                                        if (this == static_cast<rigidBody_data*>(rayCallback.m_collisionObject->getUserPointer())->_data)
                                                            //cout << "el coche soy yo mismo, shit!" << 
                                                            //        static_cast<cpuPlayer*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObject->getUserPointer())->_data)->getNombreEnPantalla() << endl;
                                                            ;
                                                        else
                                                        {
                                                            //cout << "coche delante muy cerca" << endl; 
                                                            _maniobra = estadoManiobra::COCHE_DELANTE_CERCA;
                                                        }
                                                    }
                                                    break;
                
                case tipoRigidBody::CIRCUITO:       //cout << "Rayo impacta en bordillo" << endl;
                                                    if (rayCallback.m_closestHitFraction <= 0.001)
                                                    { 
                                                        cout << "bordillo delante muy cerca" << endl;
                                                        //_maniobra = estadoManiobra::BORDILLO_CERCA;
                                                    }
                                                    break;
                                                    
                case tipoRigidBody::OBSTACULO:      // Con rayos al frente nada más por hacer 
                case tipoRigidBody::CARRETERA:
                case tipoRigidBody::CHECK_POINT: 
                default:  _maniobra = estadoManiobra::ENRUTA; break;
            }
        }
    }
}

//void cpuPlayer::rayoAtras()
//{
//    
//}

void cpuPlayer::stop()
{ 
    _onHisWay = false; 
    _car->acelerar(0); 
    //_car->frenar(); 
    _car->stop(); 
}

void cpuPlayer::cambiaMarcha()
{
    if (_marchaActual == 0)                 // Arrancamos. En este bloque if no entraremos más hasta que en otro momento marchaActual valga 0
    {                                       // Usualmente se pondrá a cero al recolocar el coche después de un accidente, salida de pista, etc.
        _timeMarcha = 0;                    // Reseteamos tiempo para empezar a contar tiempo hasta el siguiente cambio de marcha.
        _aceleracion = 0;                   // Reseteamos aceleracion. Estamos arrancando.
        _marchaActual ++;                   // Ponemos primera.
        _aceleracion = (_car->getFuerzaMotor() * ((Ogre::Real)_marchaActual/MAX_MARCHA)) * _car->getPotenciadorPrimera(); // * 2;  // Incrementamos aceleración.

    }
    else if (_timeMarcha > MAX_TIME_MARCHA)     // Si entramos aquí es que por lo menos íbamos en primera y el tiempo ha alcanzado el máximo 
    {                                           // establecido para cambiar de marcha.
        if (_marchaActual < MAX_MARCHA)        // Si no hemos llegado al tope de marchas.
        {
            _marchaActual++;
            _aceleracion = _car->getFuerzaMotor() * ((Ogre::Real)_marchaActual/MAX_MARCHA);  // Incrementamos aceleración.
        }
        else if (_marchaActual == MAX_MARCHA)
        {
            _aceleracion = _car->getFuerzaMotor() * _handicap;
        }
        
        _timeMarcha = 0;

    }
    
    _timeMarcha += _deltaT;
}

void cpuPlayer::coutTipoCollisionObject(tipoRigidBody t)
{
    string aux;
    switch (t)
    {
        case tipoRigidBody::CARRETERA: aux = "CARRETERA";   break;
        case tipoRigidBody::CHECK_POINT: aux = "CHECKPOINT"; break;
        case tipoRigidBody::CIRCUITO: aux = "CIRCUITO"; break;
        case tipoRigidBody::COCHE: aux = "COCHE"; break;
        case tipoRigidBody::OBSTACULO: aux = "OBSTACULO"; break;
        default: aux = "NI PUTA IDEA!";
    }
    
 //   cout << "tipo collisionobject: " << aux << endl;
    
}

void cpuPlayer::aplicarHandicap()
{
    if (_timeHandicap >= MAX_TIME_HANDICAP && _marchaActual == MAX_MARCHA)
    {
        _timeHandicap = 0;
        static std::random_device rdev;
        static std::default_random_engine e(rdev());

        
        if (_maniobra == estadoManiobra::COCHE_DELANTE_CERCA)
        {
            std::uniform_real_distribution<float> dis(1,1.5);
            dis.reset();
            _handicap = dis(e);
        }
        else
        {
            std::uniform_real_distribution<float> dis(0,0.8);
            dis.reset();
            _handicap = dis(e);
        }
        
    }
    else
        _handicap = 1; // sin handicap
    
    _timeHandicap += _deltaT;
}

