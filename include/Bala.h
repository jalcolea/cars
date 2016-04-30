#ifndef BALA_H
#define BALA_H

#include <Ogre.h>
#include "OgreUtil.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletCollisionsShape.h"
#include "OgreBulletDynamicsWorld.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"

#define MESH_BALA "Bala.mesh"
#define ESCALA 0.07

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


class Bala
{
public:
    Bala(shared_ptr<DynamicsWorld> world, Vector3 direccion, Vector3 posicion, SceneManager* sceneMgr, string nombre) : 
      _world(world),
      _direccion(direccion),
      _posicion(posicion),
      _sceneMgr(sceneMgr),
      _nombre(nombre)
    {
      //_posicion += Vector3(0,2,-3);
      _entBala =  _sceneMgr->createEntity(MESH_BALA);
      _nodoBala = _sceneMgr->createSceneNode();
      _nodoBala->attachObject(_entBala);
      _nodoBala->scale(ESCALA,ESCALA,ESCALA);
      _sceneMgr->getRootSceneNode()->addChild(_nodoBala);
      _nodoBala->translate(_posicion);
      _nodoBala->setVisible(true);
      _body = new RigidBody(_nombre,_world.get(),COL_BALA,COL_STAND | COL_FLOOR | COL_ROBOT | COL_ESCUDO | COL_PUERTA) ;
      _shape = new CylinderCollisionShape(_entBala->getBoundingBox().getHalfSize() * ESCALA,Vector3::UNIT_Z);
    
#ifdef _DEBUG
      _body->showDebugShape(true);
#endif

      _body->setShape(_nodoBala,                       // SceneNode que manejará Bullet
                      _shape,                          // Forma geométrica para manejar colisiones (o eso creo)
                      0.0,                             // Dynamic body restitution
                      1.0,                             // Dynamic body friction
                      1.0,                               // Dynamic body mass   
                      _posicion,                       // Posicion inicial del shape
                      Quaternion::IDENTITY);         // Orientacion del shape
      
      
      _body->enableActiveState();
      
    };

    ~Bala();
        
    void shoot(Vector3 & direccion, float fuerza, Vector3 & relPos);


private:
    shared_ptr<DynamicsWorld> _world;
    Vector3 _direccion;
    Vector3 _posicion;
    SceneManager* _sceneMgr;
    string _nombre;
    //AnimationState* _anim;
    SceneNode* _nodoBala;
    Entity* _entBala;
    RigidBody* _body;
    CylinderCollisionShape* _shape;
    //ConeCollisionShape* _shape;
    unsigned char _speed;


};

#endif // BALA_H
