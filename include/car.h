#ifndef CAR_H
#define CAR_H

#include "Ogre.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsWorld.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "Shapes/OgreBulletCollisionsCompoundShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "SceneNodeConfig.h"
#include <btBulletDynamicsCommon.h>

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


class car
{
    public :
        car() = delete;
        car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr, string material, Ogre::SceneNode* nodoPadre);
        car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr);

        virtual ~car();

        void move(int direction  , double deltaTime);
        void stop();
        btRigidBody* getBtRigidBody(){ return _body->getBulletRigidBody(); };
        SceneNode* getSceneNode(){ return _nodo; };
        void updateAnim(Ogre::Real deltaT);
        void setMaterial(const std::string & material);
        void reset();
        void setVelocity(const Real & f);
        void steer(Real r);
        inline Ogre::Vector3 getPosicion() { return _nodo->getPosition(); } // NO SANITY CHECK :D LOL Si _nodo apunta pa Utrera, casque que te crió.
        size_t & getVelocidadActualCalculada(Vector3 xInicial, Vector3 xFinal, Real deltaT);

private:
        string _nombre;
        Ogre::Vector3 _posicion;
        Ogre::SceneManager* _scnMgr;
        Ogre::SceneNode* _nodo;
        Ogre::Entity* _ent;
        SceneNodeConfig*  _scn;
        RigidBody* _body;
        string _material;
        Ogre::SceneNode* _nodoPadre;
        CollisionShape* _shape;
        ConvexHullCollisionShape* _convexShape;
        float _speed;
        int oldDir;
        size_t _velocidadCalculada;
};

#endif // CAR_H
