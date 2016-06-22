#include "cpuPlayer.h"
#include "iamanager.h"
#include "CarRayCast.h"
#include "bulletUtil.h"

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
            _iaMgr->addPunto(puntos.at(i));

        // Le pedimos al puntoManager que nos calcule puntos aleatorios para cada Checkpoint dado.
        // Cada instancia de esta clase manejará su lista de puntos aleatorios para cada Checkpoint
        // de modo que cada coche tendrá una lista ligeramente distinta y por ende una trazada también distinta.
        // Se pueden calcular varios puntos aleatorios para un mismo Checkpoint. Útil si se usa puntoManager
        // como un Singleton, de este modo cada coche podría elegir un punto aleatorio aún compartiendo la misma instancia
        // de puntoManager.
        _iaMgr->derivaPuntos(5,                  // Ancho de un checkpoint, le pasamos un ancho menor del real para que no de puntos muy cercanos a los bordes de la pista.
                             3,                   // Cuantos puntos aleatorios calcular. Para darle mayor aleatoriedad.
                             true,                // Distribuir los puntos en tantos rangos como puntos a calcular.
                             Vector3::UNIT_X,     // Sobre que eje estamos calculando los puntos aleatorios
                             0.0);                // Margen entre los rangos en los que se calculan los puntos aleatorios. 
        
        
        
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
    

    // Todos estos estados tiene sentido que inicialicen una se ha llamado a CarRayCast::buildVehiculo();
    _idCheck_destino = 0;
    _idCheck_origen = 0;
    _idCheck_meta = ((_iaMgr->getPuntos().size() - 1) * _laps);
    _finish = false;
    _sentidoContrario = false;
    _onHisWay = false;
    _timeStopped = 0;
    _timeWrongWay = 0;
    _maniobra = estadoManiobra::ENRUTA;
    _marchaActual = 0; // Marcha actual = primera :D
    _incrementoMarcha = _car->getFuerzaMotor() / MAX_MARCHA; // Incremento de velocidad al subir de marcha
    _timeMarcha = 0;   // Acumulador de tiempo para controlar cuando cambiamos de marcha;
    _aceleracion = 0;
    
    _nodoCheckPointSiguiente = nullptr;
    
    setListaPuntosAleatorios();
}

void cpuPlayer::update(Real deltaT)
{
    if (!_finish)
    {
        Vector3 origen;
        Vector3 destino;
        static Vector3 destinoOld = Vector3::ZERO; // Inicializar a Zero o a una posicion que no tenga sentido en el juego
        Vector3 direccion = Vector3::ZERO;
        Vector3 direccionActualCoche;
        btTransform btTrans;
        Real distancia;
        Quaternion quat;
        
        _deltaT = deltaT;
        
        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        compruebaCheckPoint();
        
        compruebaManiobra();
        
        destino = obtenerDestino();
        
        cambiaMarcha();

        compruebaRecolocar();
        
        if (destinoOld != destino)
        {
            //dibujaLinea(origen,destino);
            destinoOld = destino;
            _onHisWay = false;
            origen.y = destino.y;
            direccion = destino - origen;
            direccion.normalise();
        }
        
        direccionActualCoche = convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());

        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        origen.y = direccionActualCoche.y;
        direccion = destino - origen ;

        Real angulo = direccionActualCoche.getRotationTo(direccion).getYaw().valueRadians();
        cout << "angulo entre ellos = " << angulo << endl; 
        if (abs(angulo) > 0.04) // PRIMERO COMPROBAMOS QUE AL ANGULO ENTRE ELLOS ES TODAVÍA SUFICIENTEMENTE GRANDE PARA SEGUIR GIRANDO (0.02 radianes = 1º)
        {
            if (angulo > 0)    // MIRAMOS EL SIGNO PARA SABER A HACIA DONDE GIRAR.
            {
                _car->girarCPU( angulo);
            }
            else if (angulo < 0)
            {
                _car->girarCPU( angulo);
            }
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
    //static size_t i = 0;
    //https://youtu.be/nyJa-WKmWqE GRACIAS A DIOS ENCONTRÉ A ESTE CRACK. Documentación Bullet = BigShit!
    
    Vector3 inicio =  _car->getPosicionActual();
    Vector3 fin = inicio;
    fin.y += -2;

    btCollisionWorld::AllHitsRayResultCallback rayCallback(convert(inicio),convert(fin));
                                                           
    _world->getBulletDynamicsWorld()->rayTest(convert(inicio),convert(fin), rayCallback);
    
    if (rayCallback.hasHit())
    {
        for (int i=0; i<rayCallback.m_collisionObjects.size(); i++)
        {
            if (rayCallback.m_collisionObjects[i]->getUserPointer())
            {
                if (static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_tipo == tipoRigidBody::CHECK_POINT)
                {
                    size_t id = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_id;
                    _nodoCheckPointSiguiente = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_ogreNode;
                    cout << _nodoCheckPointSiguiente << endl;
                    if (_nodoCheckPointSiguiente) 
                        cout << "Nombre del sceneNode del siguiente Checkpoint" << _nodoCheckPointSiguiente->getName() << endl;
                    if (_idCheck_origen > id) // Si el id del checkpoint por el que estoy pasando es menor que el _idCheck_origen, es que hemos avanzado hacia atrás: AVISAR Y PENALIZAR.
                        _sentidoContrario = true;
                    else if (_idCheck_destino == id) // Si el idcheck_destino es igual al checkpoint por el que estoy pasando, lo hemos alcanzado.
                    {
                        _idCheck_origen = _idCheck_destino; 
                        if (_idCheck_destino == _idCheck_meta && !_finish)
                        {
                            _finish = true;
                            //stop();
                        }
                        _idCheck_destino++;
                        _idCheck_destino = _idCheck_destino % _iaMgr->getPuntos().size();
                    }
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
    Vector3 aux = _iaMgr->getPuntoDerivado(_idCheck_destino,_idxPuntoAleatorioActual);
    aux = _nodoCheckPointSiguiente->convertLocalToWorldPosition(aux);
//    cout << "punto aleatorio obtenido " << aux << endl;
    return aux; 
}

Ogre::Vector3 cpuPlayer::obtenerDestino()
{
    if (!_nodoCheckPointSiguiente) // Si no conocemos el puntero al SceneNode del checkpoint siguiente, manejamos el punto sin derivar.
        return (_iaMgr->getPunto(_idCheck_destino)).p;
    else
        if (_maniobra == estadoManiobra::COCHE_DELANTE_CERCA)
        {
            //_maniobra = estadoManiobra::ENRUTA;
            return getPuntoAleatorioEnWS(); // Sacar un punto aleatorio y convertirlo a coordenadas Globales 
        }
        else
        {
            //_maniobra = estadoManiobra::ENRUTA;
            return _iaMgr->getPunto(_idCheck_destino).p;
        }

            
}

void cpuPlayer::compruebaRecolocar()
{
     if (!_car->ruedasEnContacto() && _timeStopped > MAX_TIME_STOPPED)
    {
        //Vector3 aux = _iaMgr->getPunto(_idCheck_destino-1).p; aux.y += 0.1;
        Vector3 aux = _iaMgr->getPunto((_idCheck_destino)?(_idCheck_destino-1):_iaMgr->getPuntos().size()-1).p; aux.y += 0.1;
        _car->recolocar(aux,convert(_ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
        _maniobra = estadoManiobra::ENRUTA;
        if (_marchaActual) _marchaActual = 0;

    }                                                           // AUMENTAR LA Y PARA QUE EL COCHE NO PUEDA QUEDARSE ESTANCADO.
    else
    {
        _ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
    }
    
    if (_car->getVelocidadKmH() >= 1)
    {
        _timeStopped = 0;
        _ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
    }
    else
    {
        if (_timeStopped > MAX_TIME_STOPPED)
        {
            Vector3 aux = _iaMgr->getPunto((_idCheck_destino)?(_idCheck_destino-1):_iaMgr->getPuntos().size()-1).p; aux.y += 0.1;
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
    Vector3 inicio = _car->getPosicionActual();
    Vector3 fin = inicio + forward;
//    Vector3 backward = node->getOrientation() * Vector3(0,0,1);
//    Vector3 left= node->getOrientation() * Vector3(-1,0,0);
//    Vector3 right = node->getOrientation() * Vector3(1,0,0);
//    Vector3 up = node->getOrientation() * Vector3(0,1,0);
//    Vector3 down = node->getOrientation() * Vector3(0,-1,0);

    
    //dibujaLinea(inicio,fin,true);
    btCollisionWorld::ClosestRayResultCallback rayCallback(convert(inicio),convert(fin));
                                                           
    _world->getBulletDynamicsWorld()->rayTest(convert(inicio),convert(fin), rayCallback);
    
    if (rayCallback.hasHit())
    {
        tipoRigidBody t = static_cast<rigidBody_data*>(rayCallback.m_collisionObject->getUserPointer())->_tipo;
        switch(t)
        {
            case tipoRigidBody::COCHE:          cout << "Rayo impacta en coche " << endl;
                                                if (rayCallback.m_closestHitFraction <= 0.2)
                                                {
                                                    cout << "coche delante muy cerca" << endl; 
                                                    _maniobra = estadoManiobra::COCHE_DELANTE_CERCA;
                                                }
                                                break;
            
            case tipoRigidBody::CIRCUITO:       cout << "Rayo impacta en bordillo" << endl;
                                                if (rayCallback.m_closestHitFraction <= 0.1)
                                                { 
                                                    cout << "bordillo delante muy cerca" << endl;
                                                    _maniobra = estadoManiobra::BORDILLO_CERCA;
                                                }
                                                break;
                                                
            case tipoRigidBody::OBSTACULO:      // Con rayos al frente nada más por hacer 
            case tipoRigidBody::CARRETERA:
            case tipoRigidBody::CHECK_POINT: 
            default:  _maniobra = estadoManiobra::ENRUTA; break;
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
        _aceleracion = _car->getFuerzaMotor() * ((Ogre::Real)_marchaActual/MAX_MARCHA);  // Incrementamos aceleración.

    }
    else if (_timeMarcha > MAX_TIME_MARCHA)     // Si entramos aquí es que por lo menos íbamos en primera y el tiempo ha alcanzado el máximo 
    {                                           // establecido para cambiar de marcha.
        if (_marchaActual < MAX_MARCHA)        // Si no hemos llegado al tope de marchas.
        {
            _timeMarcha = 0;
            _marchaActual++;
            _aceleracion = _car->getFuerzaMotor() * ((Ogre::Real)_marchaActual/MAX_MARCHA);  // Incrementamos aceleración.
        }
    }
    
    _timeMarcha += _deltaT;
}
