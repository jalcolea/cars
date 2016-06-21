#include "cpuPlayer.h"
#include "iamanager.h"
#include "CarRayCast.h"
#include "bulletUtil.h"

using namespace Ogre;

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
    //_car->getSceneNode()->setOrientation(convert(quat));
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
    

}

void cpuPlayer::update(Real deltaT)
{
    if (!_finish)
    {
        Vector3 origen;
        Vector3 origenActual;
        Vector3 destino;
        static Vector3 destinoOld = Vector3::ZERO; // Inicializar a Zero o a una posicion que no tenga sentido en el juego
        Vector3 direccion = Vector3::ZERO;
        Vector3 direccionActualCoche;
        btTransform btTrans;
        Real distancia;
        Quaternion quat;
        //static Quaternion ultimaOrientacionBuena = Quaternion::IDENTITY;
        static btQuaternion ultimaOrientacionBuena;
        
        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        compruebaCheckPoint();

        // Obtenemos el punto destino al que nos dirigimos. Mientras no lo alcancemos siempre devolverá el mismo.
        srand(time(NULL));
        int aux = rand() % 2;
        if (!aux || !_nodoCheckPointSiguiente) // Si el dado vale 0, o simplemente no conocemos el puntero al SceneNode del checkpoint siguiente
            //destino = _iaMgr->vec((_iaMgr->getPoint(_idCheck_destino))->base);
            destino = (_iaMgr->getPunto(_idCheck_destino)).p;
        else
            //destino = _iaMgr->vec((_iaMgr->getPoint(_idCheck_destino))->derived);
            //destino = getPuntoAleatorioEnWS(_iaMgr->getPunto(_idCheck_destino).derivados.at(0)); 
            destino = getPuntoAleatorioEnWS(); // Sacar un punto aleatorio y convertirlo a coordenadas Globales 
        
        cout << "Destino " << this->_nombreEnPantalla << " " << destino << endl;
        
        if (!_car->ruedasEnContacto() && _timeStopped > MAX_TIME_STOPPED)
        {
            //cout << "RECOLOCANDO POR RUEDASENCONTACTO FALSE" << endl;
            //_car->recolocar(_iaMgr->vec((_iaMgr->getPoint(_idCheck_destino-1))->base),convert(ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
            _car->recolocar((_iaMgr->getPunto(_idCheck_destino-1)).p,convert(ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
        }                                                       // AUMENTAR LA Y PARA QUE EL COCHE NO PUEDA QUEDARSE ESTANCADO.
        else
        {
            //cout << "BUENA EN RUEDASENCONTACTO TRUE" << endl;
            ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
        }
        
        if (_car->getVelocidadKmH() >= 1)
        {
            //cout << "BUENA, VELOCIDAD > 1 " << endl;
            _timeStopped = 0;
            ultimaOrientacionBuena.setEuler(_car->getRigidBody()->getWorldOrientation().getYaw().valueRadians(),0,0);
        }
        else
        {
            if (_timeStopped > MAX_TIME_STOPPED)
            {
                //cout << "RECOLOCANDO, PARADO MUCHO TIEMPO" << endl;
                //_car->recolocar(origen,ultimaOrientacionBuena); // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
                _car->recolocar((_iaMgr->getPunto(_idCheck_destino-1)).p,convert(ultimaOrientacionBuena));  // CAMBIAR A ÚLTIMA DIRECCION BUENA CONOCIDA
            }
            else
            {
                //cout << "TIEMPO DE GRACIA AGOTÁNDOSE" << endl;
                _timeStopped += deltaT;
            }
        }
        
        
            
        if (destinoOld != destino)
        {
            //dibujaLinea(origen,destino);
            destinoOld = destino;
            _onHisWay = false;
            origen.y = destino.y;
            //cout << "\n\nPOSICION ACTUAL:" << origen << endl;
            //cout << "Destino al que me dirijo: " << destino << endl;
            direccion = destino - origen;
            //cout << "Vector Dirección al destino desde el punto en el que se solicita: " << direccion << endl;
            direccion.normalise();
            //cout << "Longitud direccion: " << direccion.length() << endl;
            cout << "**********************************************************************************************************************************" << endl;

        }
        
        direccionActualCoche = convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());

        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        origen.y = direccionActualCoche.y;
        direccion = destino - origen ;

        Real angulo = direccionActualCoche.getRotationTo(direccion).getYaw().valueRadians();
        //cout << "angulo entre ellos = " << angulo << endl; 
        if (abs(angulo) > 0.04) // PRIMERO COMPROBAMOS QUE AL ANGULO ENTRE ELLOS ES TODAVÍA SUFICIENTEMENTE GRANDE PARA SEGUIR GIRANDO (0.02 radianes = 1º)
        {
            if (angulo > 0)    // MIRAMOS EL SIGNO PARA SABER A HACIA DONDE GIRAR.
            {
//                cout << "orientando a izquierda" << endl;
    //            Real giro = abs((abs(angulo)/MAX_VALOR_GIRO_RUEDAS) - MAX_VALOR_GIRO_RUEDAS);
    //            cout << "giro calculado: (" << angulo << " / " << MAX_VALOR_GIRO_RUEDAS << ") - " << MAX_VALOR_GIRO_RUEDAS << " = " << giro << endl;
    //            _car->girarCPU( giro);
                _car->girarCPU( angulo);
            }
            else if (angulo < 0)
            {
//                cout << "orientando a derecha" << endl;
    //            Real giro = abs((abs(angulo)/MAX_VALOR_GIRO_RUEDAS) - MAX_VALOR_GIRO_RUEDAS);
    //            cout << "giro calculado: (" << angulo << " / " << MAX_VALOR_GIRO_RUEDAS << ") = " << giro << endl;
    //            _car->girarCPU( - giro);
                _car->girarCPU( angulo);
            }
        }
        else
        {
            //_onHisWay = true;
            _car->girarCPU(0);
//            cout << "ON_HIS_WAY INTERNO: " << _onHisWay << endl;
            //anguloDelta = 0;
            angulo = 0;
        }
        
        _car->acelerarCPU(_car->getFuerzaMotor(),false);
    }

//    Vector3 mDestination = mWalkList.front();                    // mDestination is the next location
//    Vector3 mDirection = mDestination - mNode->getPosition();    // B-A = A->B (see vector questions above)
//    Vector3 src = mNode->getOrientation() * Vector3::UNIT_X;     // see (1)
//    Real mDistance = mDirection.normalise();                     // strip out distance, we only want direction
//    Quaternion quat = src.getRotationTo(mDirection);             // Get a quaternion rotation operation 
//    mNode->rotate(quat);                                         // Actually rotate the object    
    
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
    //                cout << "Datos varios NOMBRE: " << ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_nombre << endl;
    //                cout << "Posicion del checkpoint: " << ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_worldPosition << endl;
                    //size_t id = ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_id;
                    size_t id = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_id;
                    _nodoCheckPointSiguiente = static_cast<CheckPoint_data*>(static_cast<rigidBody_data*>(rayCallback.m_collisionObjects[i]->getUserPointer())->_data)->_ogreNode;
                    cout << _nodoCheckPointSiguiente << endl;
                    if (_nodoCheckPointSiguiente) 
                        cout << "Nombre del sceneNode del siguiente Checkpoint" << _nodoCheckPointSiguiente->getName() << endl;
    //                cout << "Datos varios ID: " << id << endl;
                    if (_idCheck_origen > id) // Si el id del checkpoint por el que estoy pasando es menor que el _idCheck_origen, es que hemos avanzado hacia atrás: AVISAR Y PENALIZAR.
                        _sentidoContrario = true;
                    else if (_idCheck_destino == id) // Si el idcheck_destino es igual al checkpoint por el que estoy pasando, lo hemos alcanzado.
                    {
                        _idCheck_origen = _idCheck_destino; 
                        _idCheck_destino++;
                        if (_idCheck_destino * _laps == _idCheck_meta && !_finish)
                        {
                            _finish = true;
                            stop();
                        }
                    }
                }
            }
        }
        return true;
    }
    
    return false;
}



void cpuPlayer::dibujaLinea(Vector3 inicio, Vector3 fin)
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
}

Ogre::Vector3 cpuPlayer::getPuntoAleatorioEnWS()
{
    srand(time(nullptr));
    Vector3 aux = _iaMgr->getPunto(_idCheck_destino).derivados.at(rand()%_iaMgr->getPunto(_idCheck_destino).derivados.size());
    cout << "Coordenada aleatoria: " << aux << endl;
    aux = _nodoCheckPointSiguiente->convertLocalToWorldPosition(aux);
    cout << "Posicion aleatoria: " << aux << endl;
    return aux;
}