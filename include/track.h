#ifndef TRACK_H
#define TRACK_H

#include "Ogre.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsWorld.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "SceneNodeConfig.h"
#include "OgreUtil.h"
#include "bulletUtil.h"

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

class track
{
    public:
        track() = delete;
        track(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr) :
              _nombre(nombre), _posicion(posicionInicio), _scnMgr(scnMgr)
        {
            _scn = SceneNodeConfig::getSingletonPtr();
            nodoOgre_t nodoConfig = _scn->getInfoNodoOgre(_nombre);
            
//            StaticGeometry *stage = _scnMgr->createStaticGeometry(nodoConfig.nombreNodo);
            _ent = _scnMgr->createEntity(nodoConfig.nombreEntidad, nodoConfig.nombreMalla);
            _ent->setQueryFlags(COL_TRACK);
            _ent->setCastShadows(true);
            _nodo = _scnMgr->createSceneNode(nodoConfig.nombreNodo);
            _nodo->attachObject(_ent);
            _nodo->scale(nodoConfig.escala);
            _scnMgr->getRootSceneNode()->addChild(_nodo);
            
            //Asociar forma y cuerpo rígido (TrimeshShape y Geometría movible(attachada a un sceneNode))
            OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
            OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter->createTrimesh();
//            _body = new OgreBulletDynamics::RigidBody(_nombre, world, COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
            _body = new OgreBulletDynamics::RigidBody(_nombre, world);
            _body->setShape(_nodo,tri,nodoConfig.bodyRestitutionBullet,nodoConfig.frictionBullet,nodoConfig.masaBullet,posicionInicio);
            _body->getBulletObject()->setUserPointer(new rigidBody_data(tipoRigidBody::CIRCUITO,nullptr));
//            delete trimeshConverter;

            //Asociar forma y cuerpo rígido (CON UN CONVEXHULLCOLLISIONSHAPE y Geometría movible (attachada a un sceneNode))
//            OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//            _convexShape = trimeshConverter->createConvex();
//            _body = new OgreBulletDynamics::RigidBody(_nombre, world, COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
//            _body->setShape(_nodo,_convexShape,0.8,0.95,1000,posicionInicio);


            //Asociar forma y cuerpo rígido (CON UN SHAPE ORDINARIO)
//            OgreBulletCollisions::StaticMeshToShapeConverter trimeshConverter = OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//            _shape = trimeshConverter.createTrimesh();
//            _body = new OgreBulletDynamics::RigidBody(_nombre, world, COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
//            _body->setStaticShape(_shape, 1, 1, _posicion, Quaternion::IDENTITY);

            //Asociar forma y cuerpo rígido (CON UN TRIANGLEMESHCOLLISIONSHAPE)
//            OgreBulletCollisions::StaticMeshToShapeConverter trimeshConverter = OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//            OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter.createTrimesh();
//            _body = new OgreBulletDynamics::RigidBody(_nombre, world, COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
//            btScaledBvhTriangleMeshShape *triShape = new btScaledBvhTriangleMeshShape((btBvhTriangleMeshShape*)(tri->getBulletShape()), btVector3(1,1,1));
//            _body->setStaticShape(triShape, 1, 1, nodoConfig.posShapeBullet, Quaternion::IDENTITY);
            
            //Asociar forma y cuerpo rígido (CON UN CONVEXHULLCOLLISIONSHAPE)
//            OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//            _convexShape = trimeshConverter->createConvex();
//            _body = new OgreBulletDynamics::RigidBody(_nombre, world, COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
//            _body->setStaticShape(_convexShape, 1, 1, nodoConfig.posShapeBullet, Quaternion::IDENTITY);

            _body->setDebugDisplayEnabled(true);   
//            _body->showDebugShape(true);
            _body->enableActiveState();

//            stage->addEntity(_ent, Vector3(_posicion));
//            stage->build();


        }
              
        virtual ~track();

        btRigidBody* getBtRigidBody();
        
        inline SceneNode* getSceneNode() { return _nodo; };

    private:
        string _nombre;
        Ogre::Vector3 _posicion;
        Ogre::SceneManager* _scnMgr;
        Ogre::SceneNode* _nodo;
        Ogre::SceneNode* _nodoCol;
        Ogre::Entity* _entCol;
        Ogre::Entity* _ent;
        SceneNodeConfig*  _scn;
        RigidBody* _body;
        CollisionShape* _shape;
        ConvexHullCollisionShape* _convexShape;
};

#endif // TRACK_H
