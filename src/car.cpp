#include "car.h"
#include "OgreUtil.h"

car::car(string nombre, DynamicsWorld* world, Vector3 posicionInicio, Ogre::SceneManager* scnMgr, string material, Ogre::SceneNode* nodoPadre) : 
         _nombre(nombre), _posicion(posicionInicio), _scnMgr(scnMgr), _material(material), _nodoPadre(nodoPadre)
{
    _velocidadCalculada = 0;
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

    //_body = new RigidBody(_nombre, world, COL_CAR, COL_CAR | COL_FLOOR | COL_TRACK | COL_TRACK_COLISION);
    _body = new RigidBody(_nombre, world);


    //Asociar forma y cuerpo rígido (TrimeshShape y Geometría movible(attachada a un sceneNode)) NO FUNCIONAN LAS COLISIONES CON ESTE MODO
//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//    OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter->createTrimesh();


    //Asociar forma y cuerpo rígido (CON UN CONVEXHULLCOLLISIONSHAPE) NO SE AJUSTA BIEN AL MODELO, NI ESCALANDO EL SHAPE
    //Al intentar escalarlo se descuajaringa y crea un shape con una geometría super rara. 
//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//    _convexShape = trimeshConverter->createConvex();
//    _convexShape->getBulletShape()->setMargin(-0.05);
//    _body->setStaticShape(_convexShape, 1, 1, nodoConfig.posShapeBullet, Quaternion::IDENTITY);
    
    //Asociar forma y cuerpo rígido (CON UN COMPOUNDSHAPE) Viene a ser lo mismo que usar solo una BoxShape, pero en este caso podemos 
    //centrarla correctamente en el modelo. Así queda completamente a ras de suelo.
    OgreBulletCollisions::CompoundCollisionShape* comShape = new OgreBulletCollisions::CompoundCollisionShape();
    _shape = new BoxCollisionShape(caja);
    comShape->addChildShape(_shape, _ent->getBoundingBox().getCenter());
    _body->setShape(_nodo,
//                    _shape,
                    comShape,
//                    tri,
//                    _convexShape,
                    nodoConfig.bodyRestitutionBullet, //0.6,
                    nodoConfig.frictionBullet, //0.6,
                    nodoConfig.masaBullet,
                    nodoConfig.posInicial, // Las propiedades de bullet Posicion y Dirección sobreescriben las de Ogre, OJO
                    _nodo->getOrientation());//Quaternion::IDENTITY);
    _body->enableActiveState(); 
    _body->disableDeactivation(); // En teoría con esto no habrá que despertarlo de nuevo :D

    btTransform transform;
    transform.setIdentity();
    transform = _body->getBulletRigidBody()->getWorldTransform();
    transform.setRotation(convert(_nodo->getOrientation()));
    _body->getBulletRigidBody()->setWorldTransform(transform);
    
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

void car::setVelocity(const Real& f)
{ 
//    If you want to limit the motion of objects in the X-Z plane, and only rotation along the Y axis, use:
//    body->setLinearFactor(btVector3(1,0,1));
//    body->setAngularFactor(btVector3(0,1,0));
    
    if (_body)
    {
//        btTransform transform;
//        transform.setIdentity();
//        transform = _body->getBulletRigidBody()->getWorldTransform();
//        transform.setRotation(convert(_nodo->getOrientation()));
//        _body->getBulletRigidBody()->setWorldTransform(transform);

        //_body->getBulletRigidBody()->setAngularVelocity(btVector3(0,1,0));
        //_body->getBulletRigidBody()->setLinearVelocity(convert(_nodo->getOrientation().zAxis() * f));
        //_body->getBulletRigidBody()->applyImpulse(convert(_nodo->getOrientation().zAxis()) * f, btVector3(0,0,1));
        //_body->applyForce(_nodo->getOrientation().zAxis() * f, _body->getCenterOfMassPosition());
        _body->applyForce(_nodo->getOrientation().zAxis() * f, Vector3(0,0,0));
        
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
        //_body->enableActiveState();
    }    
}

size_t & car::getVelocidadActualCalculada(Vector3 xInicial, Vector3 xFinal, Real deltaT)
{
    
    _velocidadCalculada = (deltaT)?(int)( (xFinal - xInicial).length() / deltaT ):_velocidadCalculada;
    //cout << "Velocidad actual: " << _velocidadCalculada << " Unidades/s" << endl;
    return _velocidadCalculada;
}



//I want to cap the speed of my spaceship
//
//What's important here is two things: Firstly, doing it in a manner that doesn't go against general physics coding karma. Secondly, doing it in a way that is framerate-independant.
//To avoid messing with general physics coding karma, you should make sure you don't accidentally do this while the physics is actually stepping, and while you'd ideally like to avoid setting properties directly on the body, in this particular case you don't have a choice.
//To make sure that this is done in a manner that leads to framerate-indepedance, you need to do it *every* time that bullet ticks internally. Just waiting for stepSimulation to return is insufficient since your spaceship might be above the max velocity for multiple internal ticks.
//In short, the best way to do this is by setting the velocity of your spaceship in the physics tick callback.
//void myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
//    // mShipBody is the spaceship's btRigidBody
//    btVector3 velocity = mShipBody->getLinearVelocity();
//    btScalar speed = velocity.length();
//    if(speed > mMaxSpeed) {
//        velocity *= mMaxSpeed/speed;
//        mShipBody->setLinearVelocity(velocity);
//    }
//}