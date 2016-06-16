#include "cpuPlayer.h"
#include "iamanager.h"
#include "CarRayCast.h"

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
    _car->getSceneNode()->setOrientation(convert(quat));
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
    

}

void cpuPlayer::update(Real deltaT)
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
    static Real anguloDelta = 0;
    
    btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
    origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
    
    compruebaCheckPoint();

    // Obtenemos el punto destino al que nos dirigimos. Mientras no lo alcancemos siempre devolverá el mismo.
    destino = _iaMgr->vec((_iaMgr->getPoint(_idCheck_destino))->base);
    
    
    
    if (destinoOld != destino)
    {
        dibujaLinea(origen,destino);
        destinoOld = destino;
        _onHisWay = false;
        origen.y = destino.y;
        cout << "\n\nPOSICION ACTUAL:" << origen << endl;
        cout << "Destino al que me dirijo: " << destino << endl;
        direccion = destino - origen;
        cout << "Vector Dirección al destino desde el punto en el que se solicita: " << direccion << endl;
        //direccion.normalise();
        cout << "Longitud direccion: " << direccion.length() << endl;
        cout << "**********************************************************************************************************************************" << endl;

    }
    
    if (!_onHisWay)
    {
        direccionActualCoche = convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());

        btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
        origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
        
        origen.y = direccionActualCoche.y;
        direccion = destino - origen ;

        Real angulo = direccionActualCoche.getRotationTo(direccion).getYaw().valueRadians();
            cout << "angulo entre ellos = " << angulo << endl; 
            if (abs(angulo) > 0.06) // PRIMERO COMPROBAMOS QUE AL ANGULO ENTRE ELLOS ES TODAVÍA SUFICIENTEMENTE GRANDE PARA SEGUIR GIRANDO (0.02 radianes = 1º)
            {
                    if (angulo > 0)    // MIRAMOS EL SIGNO PARA SABER A HACIA DONDE GIRAR.
                    {
                        cout << "orientando a izquierda" << endl;
                        Real giro = abs((abs(angulo)/MAX_VALOR_GIRO_RUEDAS) - MAX_VALOR_GIRO_RUEDAS);
                        //Real giro = abs(Math::Clamp(angulo/MAX_VALOR_GIRO_RUEDAS,-MAX_VALOR_GIRO_RUEDAS,MAX_VALOR_GIRO_RUEDAS));
                        cout << "giro calculado: (" << angulo << " / " << MAX_VALOR_GIRO_RUEDAS << ") - " << MAX_VALOR_GIRO_RUEDAS << " = " << giro << endl;
                        _car->girarCPU( giro);
                        //_car->girarCPU( MAX_VALOR_GIRO_RUEDAS);
                    }
                    else if (angulo < 0)
                    {
                        cout << "orientando a derecha" << endl;
                        Real giro = abs((abs(angulo)/MAX_VALOR_GIRO_RUEDAS) - MAX_VALOR_GIRO_RUEDAS);
                        //Real giro = abs(Math::Clamp(angulo/MAX_VALOR_GIRO_RUEDAS,-MAX_VALOR_GIRO_RUEDAS,MAX_VALOR_GIRO_RUEDAS));
                        cout << "giro calculado: (" << angulo << " / " << MAX_VALOR_GIRO_RUEDAS << ") = " << giro << endl;
                        _car->girarCPU( - giro);
                        //_car->girarCPU( - MAX_VALOR_GIRO_RUEDAS);
                    }
                }
                else
                {
                    //_onHisWay = true;
                    _car->girarCPU(0);
                    cout << "ON_HIS_WAY INTERNO: " << _onHisWay << endl;
                    anguloDelta = 0;
                    angulo = 0;
                }
    }
    
    _car->acelerarCPU(_car->getFuerzaMotor(),_onHisWay);


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
            //rayCallback.m_collisionObjects[i]->getCollisionShape()
            if (rayCallback.m_collisionObjects[i]->getUserPointer())
            {
//                cout << "Datos varios NOMBRE: " << ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_nombre << endl;
//                cout << "Posicion del checkpoint: " << ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_worldPosition << endl;
                size_t id = ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_id;
//                cout << "Datos varios ID: " << id << endl;
                if (_idCheck_origen > id) // Si el id del checkpoint por el que estoy pasando es menor que el _idCheck_origen, es que hemos avanzado hacia atrás: AVISAR Y PENALIZAR.
                    _sentidoContrario = true;
                else if (_idCheck_destino == id) // Si el idcheck_destino es igual al checkpoint por el que estoy pasando, lo hemos alcanzado.
                {
                    _idCheck_origen = _idCheck_destino; 
                    _idCheck_destino++;
                    if (_idCheck_destino * _laps == _idCheck_meta)
                        _finish = true;
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