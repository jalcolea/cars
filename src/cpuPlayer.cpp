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
//    btTransform trans = _car->getRigidBody()->getBulletRigidBody()->getCenterOfMassTransform();
//    btQuaternion quat;
//    quat.setEuler(Radian(Ogre::Degree(-90)).valueRadians(),0,0);
//    trans.setRotation(quat);
//    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
    

}

void cpuPlayer::update(Real deltaT)
{
    Vector3 destino;
    Vector2 destino2;
    Vector3 direccion;
    Vector2 direccion2;
    btTransform btTrans = _car->getVehiculo()->getBulletVehicle()->getChassisWorldTransform();
    Real distancia;
    Quaternion quat;
    
    
    
    destino = _iaMgr->vec(_iaMgr->follow(new iapoint())->base);
    destino2 = Vector2(destino.x,destino.z);
    Vector3 origen = Vector3(btTrans.getOrigin().getX(),destino.y,btTrans.getOrigin().getZ()) * Vector3::UNIT_X;
    Vector2 origen2(origen.x,origen.z);

    cout << "CURIOSIDAD :D " << origen2.angleBetween(destino2).valueRadians() << endl;

    Quaternion orientacionActual = convert(btTrans.getRotation());// convert(_car->getRigidBody()->getBulletRigidBody()->getOrientation());
    cout << "orientacion actual :" << orientacionActual << endl;
    origen = orientacionActual * Vector3::UNIT_X;
    cout << "origen: " << origen << endl;
    direccion = (destino - origen) * -convert(_car->getVehiculo()->getBulletVehicle()->getForwardVector());
    cout << "direccion: " << direccion << endl;
    direccion.normalise();
    cout << "direccion normalizada:" << direccion << endl;
    quat = origen.getRotationTo(direccion);
    cout << "quat(origen.getRotationTo): " << quat << endl;
    
    
    
    btTrans.setRotation(convert(quat));
    
    cout << "orientacion despues de calcular: " << Ogre::Degree(convert(btTrans.getRotation()).getYaw()).valueDegrees() << endl;
//    _car->getVehiculo()->setTransform();
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(btTrans);


    _car->girar(1);
    _car->acelerar(_car->getFuerzaMotor(),true);



            //  Vector3 mDestination = mWalkList.front();                    // mDestination is the next location
            //  Vector3 mDirection = mDestination - mNode->getPosition();    // B-A = A->B (see vector questions above)
            //  Vector3 src = mNode->getOrientation() * Vector3::UNIT_X;     // see (1)
            //  Real mDistance = mDirection.normalise();                     // strip out distance, we only want direction
            //  Quaternion quat = src.getRotationTo(mDirection);             // Get a quaternion rotation operation 
            //
            //  mNode->rotate(quat);                                         // Actually rotate the object    
    
    
    
}

