#ifndef TRACK_H
#define TRACK_H

#include "Ogre.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsWorld.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "SceneNodeConfig.h"
#include "OgreUtil.h"

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
            
            StaticGeometry *stage = _scnMgr->createStaticGeometry(nodoConfig.nombreNodo);
            _ent = _scnMgr->createEntity(nodoConfig.nombreEntidad, nodoConfig.nombreMalla);
            _ent->setQueryFlags(COL_TRACK);
            _ent->setCastShadows(true);
            //Asociar forma y cuerpo rígido
            OgreBulletCollisions::StaticMeshToShapeConverter trimeshConverter = OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
            _shape = trimeshConverter.createTrimesh();
            _body = new OgreBulletDynamics::RigidBody(_nombre, world, COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
            _body->setStaticShape(_shape, 1, 1, _posicion, Quaternion::IDENTITY);

            _ent->setCastShadows(true);
            stage->addEntity(_ent, Vector3(_posicion));
            stage->build();
        }
              
        virtual ~track();

        btRigidBody* getBtRigidBody();

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
};

#endif // TRACK_H
