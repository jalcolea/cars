#include "cpuPlayer.h"
#include "iamanager.h"


cpuPlayer::~cpuPlayer()
{
}

void cpuPlayer::build()
{
    _car->buildVehiculo(); 
    btTransform trans = _car->getRigidBody()->getBulletRigidBody()->getCenterOfMassTransform();
    btQuaternion quat;
    quat.setEuler(Radian(Ogre::Degree(-90)).valueRadians(),0,0);
    trans.setRotation(quat);
    _car->getRigidBody()->getBulletRigidBody()->setWorldTransform(trans);
}

void cpuPlayer::update(Real deltaT)
{
//    Vector3 posActual = _car->getSceneNode()->getPosition();
//    _iaMgr->next(posActual.x,posActual.y,posActual.z);
    
    
    
    
}

