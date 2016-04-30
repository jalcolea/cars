#include "Bala.h"


Bala::~Bala()
{
}

void Bala::shoot(Vector3 & direccion, float fuerza, Vector3 & relPos)
{
    _body->enableActiveState();
    //_body->applyImpulse(direccion * fuerza, relPos);
    //_body->setLinearVelocity(direccion * fuerza);
    _nodoBala->lookAt(direccion,Ogre::Node::TS_LOCAL);
    
    btTransform transform;
    transform.setIdentity();
    transform = _body->getBulletRigidBody()->getWorldTransform();
    transform.setRotation(convert(_nodoBala->getOrientation()));
    _body->getBulletRigidBody()->setWorldTransform(transform);
    
    _body->getBulletRigidBody()->setLinearVelocity(convert(direccion * fuerza));

    
}

