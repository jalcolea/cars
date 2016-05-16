#include "car.h"
#include "OgreUtil.h"


car::car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr, string material, Ogre::SceneNode* nodoPadre) : 
         _nombre(nombre), _posicion(posicionInicio), _scnMgr(scnMgr), _material(material), _nodoPadre(nodoPadre)
{
    nodoOgre_t nodoConfig;
    _scn = SceneNodeConfig::getSingletonPtr();
    nodoConfig = _scn->getInfoNodoOgre("carKartWhite"); // Sustituir por un parámetro que indique el coche que queremos 
    _nodo = _scnMgr->createSceneNode(nodoConfig.nombreNodo);
    _ent = _scnMgr->createEntity(nodoConfig.nombreEntidad,nodoConfig.nombreMalla);
    _ent->setCastShadows(true);
    if (material.length()) _ent->setMaterialName(material);
    _nodo->attachObject(_ent);
    if (_nodoPadre) _nodoPadre->addChild(_nodo); else _scnMgr->getRootSceneNode()->addChild(_nodo);
    _nodo->setPosition(nodoConfig.posInicial);
    _nodo->setOrientation(nodoConfig.orientacion);

    _body = new  RigidBody(_nombre, world,COL_CAR,COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION);
    _shape = new BoxCollisionShape(_ent->getBoundingBox().getHalfSize());
    _body->setShape(_nodo,
                    _shape,
                    0.0,
                    0.0,
                    0.1,
                    nodoConfig.posInicial, // Las propiedades de bullet Posicion y Dirección sobreescriben las de Ogre, OJO
                    _nodo->getOrientation());//Quaternion::IDENTITY);
    _body->enableActiveState();
    
    btTransform transform;
    transform.setIdentity();
    transform = _body->getBulletRigidBody()->getWorldTransform();
    transform.setRotation(convert(_nodo->getOrientation()));
    _body->getBulletRigidBody()->setWorldTransform(transform);
    
    //_body->getBulletRigidBody()->setLinearVelocity(convert(direccion * fuerza));
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


