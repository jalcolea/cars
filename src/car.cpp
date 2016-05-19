#include "car.h"
#include "OgreUtil.h"

/*
car::car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr, string material, Ogre::SceneNode* nodoPadre) : 
         _nombre(nombre), _posicion(posicionInicio), _scnMgr(scnMgr), _material(material), _nodoPadre(nodoPadre)
{
    nodoOgre_t nodoConfig;
    _scn = SceneNodeConfig::getSingletonPtr();
    nodoConfig = _scn->getInfoNodoOgre(nombre); // Sustituir por un parámetro que indique el coche que queremos 
    _nodo = _scnMgr->createSceneNode(nodoConfig.nombreNodo);
    _ent = _scnMgr->createEntity(nodoConfig.nombreEntidad,nodoConfig.nombreMalla);
    _ent->setCastShadows(true);
    if (material.length()) _ent->setMaterialName(material);
    _nodo->attachObject(_ent);
    if (_nodoPadre) _nodoPadre->addChild(_nodo); else _scnMgr->getRootSceneNode()->addChild(_nodo);
    _nodo->setPosition(nodoConfig.posInicial);
    _nodo->setOrientation(nodoConfig.orientacion);
    
    Ogre::Vector3 caja = _ent->getBoundingBox().getHalfSize();
//    caja *= 0.96;
//    caja.y*=0.2;
//    caja.x*=0.9;
//    caja.z*=0.9;

    _body = new  RigidBody(_nombre, world, COL_CAR, COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION);
    _shape = new BoxCollisionShape(caja); 
    
    OgreBulletCollisions::CompoundCollisionShape* comShape = new OgreBulletCollisions::CompoundCollisionShape();
    comShape->addChildShape(_shape, _ent->getBoundingBox().getCenter());
    _body->setShape(_nodo,
//                    _shape,
                    comShape,
                    0.6,
                    0.6,
                    100,
                    nodoConfig.posShapeBullet, // Las propiedades de bullet Posicion y Dirección sobreescriben las de Ogre, OJO
                    _nodo->getOrientation());//Quaternion::IDENTITY);
    _body->enableActiveState();
    
//    btTransform transform;
//    transform.setIdentity();
//    transform = _body->getBulletRigidBody()->getWorldTransform();
//    transform.setRotation(convert(_nodo->getOrientation()));
//    _body->getBulletRigidBody()->setWorldTransform(transform);
    
    _body->getBulletRigidBody()->setLinearVelocity(convert(Ogre::Vector3(0,0,10)));
}
*/

car::car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr, string material, Ogre::SceneNode* nodoPadre) : 
         _nombre(nombre), _posicion(posicionInicio), _scnMgr(scnMgr), _material(material), _nodoPadre(nodoPadre)
{
    nodoOgre_t nodoConfig;
    _scn = SceneNodeConfig::getSingletonPtr();
    nodoConfig = _scn->getInfoNodoOgre(nombre); // Sustituir por un parámetro que indique el coche que queremos 
    _nodo = _scnMgr->createSceneNode(nodoConfig.nombreNodo);
    _ent = _scnMgr->createEntity(nodoConfig.nombreEntidad,nodoConfig.nombreMalla);
    _ent->setCastShadows(true);
    if (material.length()) _ent->setMaterialName(material);
    _nodo->attachObject(_ent);
    if (_nodoPadre) _nodoPadre->addChild(_nodo); else _scnMgr->getRootSceneNode()->addChild(_nodo);
    _nodo->setPosition(nodoConfig.posInicial);
    _nodo->setOrientation(nodoConfig.orientacion);
    
    Ogre::Vector3 caja = _ent->getBoundingBox().getHalfSize();
    caja *= 0.96;

    _body = new RigidBody(_nombre, world, COL_CAR, COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION);
//    _shape = new BoxCollisionShape(caja);

    //Asociar forma y cuerpo rígido (TrimeshShape y Geometría movible(attachada a un sceneNode))
//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//    OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter->createTrimesh();


    //Asociar forma y cuerpo rígido (CON UN CONVEXHULLCOLLISIONSHAPE)
    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
    _convexShape = trimeshConverter->createConvex();
    _body->setStaticShape(_convexShape, 1, 1, nodoConfig.posShapeBullet, Quaternion::IDENTITY);
    
//    OgreBulletCollisions::CompoundCollisionShape* comShape = new OgreBulletCollisions::CompoundCollisionShape();
//    comShape->addChildShape(static_cast<OgreBulletCollisions::BoxCollisionShape*>(_shape), _ent->getBoundingBox().getCenter());
    _body->setShape(_nodo,
//                    _shape,
//                    comShape,
//                    tri,
                    _convexShape,
                    0.6,
                    0.6,
                    500,
                    nodoConfig.posInicial, // Las propiedades de bullet Posicion y Dirección sobreescriben las de Ogre, OJO
                    _nodo->getOrientation());//Quaternion::IDENTITY);
    _body->enableActiveState();



//    btTransform transform;
//    transform.setIdentity();
//    transform = _body->getBulletRigidBody()->getWorldTransform();
//    transform.setRotation(convert(_nodo->getOrientation()));
//    _body->getBulletRigidBody()->setWorldTransform(transform);
    
}



car::car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr) : 
         car(nombre,world, posicionInicio, scnMgr, "", nullptr) {}





car::~car()
{
}

void car::setMaterial(const std::string & material)
{
    if (_ent)
        _ent->setMaterialName(_material);
}

void car::setVelocity(Real f)
{ 
    if (_body)
    {
        _body->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
        btTransform transform;
        transform.setIdentity();
        transform = _body->getBulletRigidBody()->getWorldTransform();
        transform.setRotation(convert(_nodo->getOrientation()));
        _body->getBulletRigidBody()->setWorldTransform(transform);
        _body->enableActiveState();
        _body->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
        _body->getBulletRigidBody()->setLinearVelocity(convert(_nodo->getOrientation().zAxis() * f));
    }
}

void car::steer(Real r)
{
    if (_body)
    {
        _nodo->yaw(Radian(r));
        _body->getBulletRigidBody()->setAngularVelocity(btVector3(0,1,0));
        btTransform transform;
        transform.setIdentity();
        transform = _body->getBulletRigidBody()->getWorldTransform();
        transform.setRotation(convert(_nodo->getOrientation()));
        _body->getBulletRigidBody()->setWorldTransform(transform);
        _body->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));        
        _body->enableActiveState();
    }    
}


