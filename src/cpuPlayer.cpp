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
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
    

}

void cpuPlayer::update(Real deltaT)
{
    Vector3 origen;
    Vector3 destino;
    static Vector3 destinoOld = Vector3::ZERO;
    Vector3 direccion;
    btTransform btTrans;
    Real distancia;
    Quaternion quat;
    

//  INTENTANDO ORIENTAR EL CHASIS. DE MOMENTO SE ORIENTA PERO LA ROTACIÓN LA HACE AL CONTRARIO DE LO QUE DEBERÍA
//    btTrans.setIdentity(); // Por si acaso, la matriz de transformación la dejamos Inmaculadita para empezar :D
//    btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
//    destino = _iaMgr->vec(_iaMgr->follow(new iapoint())->base); // Obtenemos el punto destino al que nos dirigimos. Mientras no lo alcancemos siempre devolverá el mismo.
//    origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
//    origen.normalise();    // Normalizamos ambos vectores, de lo contrario los cálculos de ángulos y demás se vuelven locos.
//    destino.normalise();
//    destino.y = origen.y; // igualo la y para que no la tenga en cuenta al calcular el angulo de rotación entre origen y destino
//    Quaternion orientacionActual = convert(btTrans.getRotation()); // Obtenemos el cuaternio con la rotación actual del chasis desde su matriz de transformacion.
//    origen = orientacionActual * Vector3::UNIT_X; // Producto vectorial para deshacernos de las componentes que no nos interesan (X,Z)
//    direccion = (destino - origen) *  -convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());// * Vector3::UNIT_X;
//    //direccion.normalise();
//    quat = origen.getRotationTo(direccion);
//    cout << "yaw resultante " << quat.getYaw().valueAngleUnits() << endl;
//    btTrans.setRotation(convert(quat));
//    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(btTrans);

    btTrans.setIdentity(); // Por si acaso, la matriz de transformación la dejamos Inmaculadita para empezar :D
    btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform(); // Obtenemos la matriz de transformación del chasis
    origen = convert(btTrans.getOrigin()); // Obtenemos la posición actual del chasis 
    
    compruebaCheckPoint();

//    destino = _iaMgr->vec(_iaMgr->follow(new iapoint(origen.x,origen.y,origen.z))->derived); // Obtenemos el punto destino al que nos dirigimos. Mientras no lo alcancemos siempre devolverá el mismo.
//    
//    cout << "Destino actual " << destino << endl;
//    if (destino != destinoOld)
//    {
//        cout << "Destino alcanzado, orientando a siguiente destino " << endl;
//        destinoOld = destino;
//        //origen.normalise();    // Normalizamos ambos vectores, de lo contrario los cálculos de ángulos y demás se vuelven locos.
//        //destino.normalise();
//        destino.y = origen.y; // igualo la y para que no la tenga en cuenta al calcular el angulo de rotación entre origen y destino
//        Vector3 orientacionActual = convert(btTrans.getRotation()) * Vector3::UNIT_X; // Obtenemos la rotación actual del chasis desde su matriz de transformacion.
//        //origen = orientacionActual * Vector3::UNIT_X; // Producto vectorial para deshacernos de las componentes que no nos interesan (X,Z)
//        direccion = (destino - origen); //* -convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());// * Vector3::UNIT_X;
//        direccion.normalise();
//        quat = orientacionActual.getRotationTo(direccion); //origen.getRotationTo(direccion);
//        cout << "yaw resultante " << quat.getYaw().valueAngleUnits() << endl;
//        btTrans.setRotation(convert(quat));
//        _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(btTrans);
//    }


    _car->acelerar(_car->getFuerzaMotor(),false,0.1);



// OTRA APROXIMACIÓN PARA LA ORIENTACION, NO FUNCIONA. SE INTENTA GIRAR EL COCHE HASTA QUE SE ALINEE CON EL PUNTO DE LA MARCA
// PERO COMIENZA A GIRAR EN ESPIRAL HASTA QUE LA FUERZA CENTRÍPETA HACE QUE EL COCHE VUELQUE, PA CORTARSE LAS VENAS YA :(
//    Vector3 origen = _car->getPosicionActual();
//    origen.y = destino.y;
//    cout << "angulo entre ellos (angleBetween) = " << origen.angleBetween(destino).valueAngleUnits() << endl; // DEVUELVE VALORES SIN SIGNO, NO ME VALE
//    cout << "angulo entre ellos (getRotationTo) = " << origen.getRotationTo(destino).getYaw().valueAngleUnits() << endl;
//    Real anguloDelta = origen.getRotationTo(destino).getYaw().valueAngleUnits();
//    if (anguloDelta > 0)
//    {
//        cout << "orientando a derecha" << endl;
//        _car->girar(-1,0.1);
//    }
//    else if (anguloDelta < 0)
//    {
//        cout << "orientando a izquierda" << endl;
//        _car->girar(1,0.1);
//    }
    

//    ALTERNATIVA 2 PARA INTENTAR ORIENTAR EL VEHÍCULO    
//    body.getWorldTransform().getRotation(mRotation);
//    float roll = mRotation.getRoll();
//    float angle = mRotation.getAngleAround(0, 0, 1);
//    int gimbalPole = mRotation.getGimbalPole();
//    float rotation = (gimbalPole == 0) ? roll : angle * gimbalPole;

//    ALTERNATIVA 1 PARA INTENTAR ORIENTAR EL VEHÍCULO (NO FUNCIONA BIEN)
//    Vector3 mDestination = mWalkList.front();                    // mDestination is the next location
//    Vector3 mDirection = mDestination - mNode->getPosition();    // B-A = A->B (see vector questions above)
//    Vector3 src = mNode->getOrientation() * Vector3::UNIT_X;     // see (1)
//    Real mDistance = mDirection.normalise();                     // strip out distance, we only want direction
//    Quaternion quat = src.getRotationTo(mDirection);             // Get a quaternion rotation operation 
//    mNode->rotate(quat);                                         // Actually rotate the object    
    
}


bool cpuPlayer::compruebaCheckPoint()
{
    static size_t i = 0;
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
                cout << "Datos varios NOMBRE: " << ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_nombre << endl;
                cout << "Datos varios ID: " << ((CheckPoint_data*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->_id << endl;
            }
        }
        return true;
    }
    
    return false;
}