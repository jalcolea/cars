#include <iostream>
#include "testStateVehicRayCast.h"
#include "PlayState.h"
#include "MenuState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include <bitset>
#include <Ogre.h>
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

#include "CarRayCast.h"

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

template<> testStateVehicRayCast *Ogre::Singleton<testStateVehicRayCast>::msSingleton = 0;

struct datosVarios
{
    datosVarios(string n, int i):nombre(n),id(i){}
    string nombre;
    int id;
};

void testStateVehicRayCast::enter()
{
    // Recuperar recursos básicos
    _root = Ogre::Root::getSingletonPtr();
    try
    {
        _sceneMgr = _root->getSceneManager("SceneManager");
    }
    catch (...)
    {
        cout << "SceneManager no existe, creándolo \n";
        _sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
    }

    try
    {
        _camera = _sceneMgr->getCamera("IntroCamera");
    }
    catch (...)
    {
        cout << "testCamera no existe, creándola \n";
        _camera = _sceneMgr->createCamera("IntroCamera");
    }

    try
    {
        _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
    }
    catch (...)
    {
        _viewport = _root->getAutoCreatedWindow()->getViewport(0);
    }


    //Color de fondo inicial
    _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));

    // Cargar los parámetros para construir elementos del juego (coches, circuitos, camaras, etc)
    cargarParametros("SceneNodes.xml",true);
    
    // Configurar camara
    configurarCamaraPrincipal();

    // Activar Bullet
    initBulletWorld(false);

    //Preparar escena
    createScene();
    
    _exitGame = false;
    _deltaT = 0;
    sounds::getInstance()->play_music("begin");

}

void testStateVehicRayCast::exit()
{
//    sounds::getInstance()->halt_music();
//    destroyMyGui();
    _sceneMgr->clearScene();
    _root->getAutoCreatedWindow()->removeAllViewports();
}

void testStateVehicRayCast::pause()
{
}

void testStateVehicRayCast::resume()
{

}

bool testStateVehicRayCast::frameStarted(const Ogre::FrameEvent &evt)
{
    _deltaT = evt.timeSinceLastFrame;
    if (_playSimulation) _world.get()->stepSimulation(_deltaT);
    static Vector3 oldPos = _car->getPosicion();
    static Ogre::Real speed = 10.0;
    _fps = 1.0 / _deltaT;
    _r = 0;
    Real rr = 0;
    Real rSteer = 0;
    Real sCar = 0;
    static Real sBrake = 10;
    _vt = Ogre::Vector3::ZERO;

    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_P)) _playSimulation = !_playSimulation;

    _velocidad = _car->getVelocidadActualCalculada(oldPos,_car->getPosicion(),_deltaT);

    if (oldPos != _car->getPosicion())
        oldPos = _car->getPosicion();

    if (_keys & static_cast<size_t>(keyPressed_flags::LEFT))  _vt.x += -1;
    if (_keys & static_cast<size_t>(keyPressed_flags::RIGHT)) _vt.x += 1;
    if (_keys & static_cast<size_t>(keyPressed_flags::UP))    _vt.y += 1;
    if (_keys & static_cast<size_t>(keyPressed_flags::DOWN))  _vt.y += -1;
    if (_keys & static_cast<size_t>(keyPressed_flags::INS))   _vt.z += 1;
    if (_keys & static_cast<size_t>(keyPressed_flags::DEL))   _vt.z += -1;
    
    if (_keys & static_cast<size_t>(keyPressed_flags::NUMPAD5))
    {
        if ((_keys & static_cast<size_t>(keyPressed_flags::NUMPAD1)) || (_keys & static_cast<size_t>(keyPressed_flags::NUMPAD3)))
           _vCarsRayCast[_cursorVehiculo]->acelerar(_vCarsRayCast[_cursorVehiculo]->getFuerzaMotor(),false);
        else 
            _vCarsRayCast[_cursorVehiculo]->acelerar(_vCarsRayCast[_cursorVehiculo]->getFuerzaMotor(),true);
    }
    else _vCarsRayCast[_cursorVehiculo]->acelerar(0); // Si no aceleramos que actúe la inercia. También sirve para cuando soltamos el freno :D

    if (_keys & static_cast<size_t>(keyPressed_flags::NUMPAD2)) _vCarsRayCast[_cursorVehiculo]->frenar();
    
    if (_keys & static_cast<size_t>(keyPressed_flags::NUMPAD1)) 
        _vCarsRayCast[_cursorVehiculo]->girar(1);
    if (_keys & static_cast<size_t>(keyPressed_flags::NUMPAD3)) 
        _vCarsRayCast[_cursorVehiculo]->girar(-1);
    
    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_HOME)) speed =0.5;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_END)) speed =10;

//    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_W))
//        _car->setVelocity(10);
//    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_A))
//        _car->steer((rSteer+=180) * _deltaT * 0.08);
//    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_D))
//        _car->steer((rSteer-=180) * _deltaT * 0.08);
        
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_W))
        _car->setVelocity(1500); sBrake = 1500; 
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_S))
        _car->setVelocity(sBrake-=1500*_deltaT);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_A))
        _car->steer((rSteer+=180) * _deltaT * 0.2);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_D))
        _car->steer((rSteer-=180) * _deltaT * 0.2);
    

    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_0))
        _world.get()->setShowDebugShapes(!_world.get()->getShowDebugShapes());  // Casca miserablemente :( manda...

    _camera->moveRelative(_vt * _deltaT * speed);//10.0 /*tSpeed*/);-l
    if (_camera->getPosition().length() < 2.0) 
        _camera->moveRelative(-_vt * _deltaT * speed);//10.0 /*tSpeed*/);
        
    if (_keys & static_cast<size_t>(keyPressed_flags::PGUP)) _r += 180;
    if (_keys & static_cast<size_t>(keyPressed_flags::PGDOWN)) _r += -180;
        
    _camera->pitch(Ogre::Radian(_r * _deltaT * 0.005));
    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_8)) rr+=180;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_9)) rr-=180;
    _camera->yaw(Ogre::Radian(rr * _deltaT * 0.005));

    if (!_freeCamera)
        reposicionaCamara();

    pintaOverlayInfo();
    
    compruebaCheckPoint();
    
    if (!_vCarsRayCast[_cursorVehiculo]->ruedasEnContacto())
        cout << "EL COCHE HA VOLCADO!!!!!!!!!!????????" << endl;
    

    return !_exitGame;

}


void testStateVehicRayCast::reposicionaCamara()
{
    switch(_vista)
    {
        /*case camara_view::SEMICENITAL:  _camera->setPosition(_carRayCast->getPosicionActual().x,
                                                             _carRayCast->getPosicionActual().y +10 ,
                                                             _carRayCast->getPosicionActual().z + 30);  break; */
        case camara_view::SEMICENITAL:  _camera->setPosition(_vCarsRayCast[_cursorVehiculo]->getPosicionActual().x,
                                                             _vCarsRayCast[_cursorVehiculo]->getPosicionActual().y +10 ,
                                                             _vCarsRayCast[_cursorVehiculo]->getPosicionActual().z + 30);  break;
        case camara_view::TRASERA_BAJA: break;
        case camara_view::TRASERA_ALTA: break;
        case camara_view::INTERIOR:     break;
        default: assert(true);                                                            

    }
    
}
/*
void testStateVehicRayCast::colocaCamara()
{
    cout << "padre de atacheo de camara: " << _camera->getParentNode() << endl;
    switch (_vista)
    {
        case camara_view::SEMICENITAL:  {_nodoVista->detachObject(_camera);
                                        _carRayCast->getSceneNode()->removeChild(_nodoVista);
                                        nodoCamera_t cam = _scn.getInfoCamera("IntroCamera");
                                        _camera->setPosition(cam.posInicial);
        _camera->lookAt(_carRayCast->getPosicionActual()); }
                                        break;
        case camara_view::TRASERA_BAJA: _camera->setPosition(_carRayCast->getPosicionActual().x,
                                                      _carRayCast->getPosicionActual().y + 3,
                                                      _carRayCast->getPosicionActual().z - 5); 
                                                      _camera->lookAt(_carRayCast->getPosicionActual()); break;
        case camara_view::TRASERA_ALTA: _camera->setPosition(_carRayCast->getPosicionActual().x,
                                                      _carRayCast->getPosicionActual().y + 5,
                                                      _carRayCast->getPosicionActual().z - 5); 
                                                      _camera->lookAt(_carRayCast->getPosicionActual()); break;
        case camara_view::INTERIOR:     _nodoVista->attachObject(_camera);
                                        _carRayCast->getSceneNode()->addChild(_nodoVista);
//                                      _camera->setPosition(0,0.13,-0.15); // Camara interior
                                        _camera->setPosition(0,1.5,-3);
                                        //_camera->yaw(Ogre::Degree(180));

        case camara_view::TOTAL_COUNT:                                                
        default: assert(true);
    }   
    
                            
}
*/
void testStateVehicRayCast::colocaCamara()
{
    cout << "padre de atacheo de camara: " << _camera->getParentNode() << endl;
    switch (_vista)
    {
        case camara_view::SEMICENITAL:  {_nodoVista->detachObject(_camera);
                                        _vCarsRayCast[_cursorVehiculo]->getSceneNode()->removeChild(_nodoVista);
                                        nodoCamera_t cam = SceneNodeConfig::getSingleton().getInfoCamera("IntroCamera");
                                        _camera->setPosition(cam.posInicial);
        _camera->lookAt(_vCarsRayCast[_cursorVehiculo]->getPosicionActual()); }
                                        break;
        case camara_view::TRASERA_BAJA: _camera->setPosition(_vCarsRayCast[_cursorVehiculo]->getPosicionActual().x,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().y + 3,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().z - 5); 
                                                      _camera->lookAt(_vCarsRayCast[_cursorVehiculo]->getPosicionActual()); break;
        case camara_view::TRASERA_ALTA: _camera->setPosition(_vCarsRayCast[_cursorVehiculo]->getPosicionActual().x,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().y + 5,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().z - 5); 
                                                      _camera->lookAt(_vCarsRayCast[_cursorVehiculo]->getPosicionActual()); break;
        case camara_view::INTERIOR:     _nodoVista->attachObject(_camera);
                                        _vCarsRayCast[_cursorVehiculo]->getSceneNode()->addChild(_nodoVista);
//                                      _camera->setPosition(0,0.13,-0.15); // Camara interior
                                        _camera->setPosition(0,1.5,-3);
                                        //_camera->yaw(Ogre::Degree(180));

        case camara_view::TOTAL_COUNT:                                                
        default: assert(true);
    }   
    
                            
}


void testStateVehicRayCast::pintaOverlayInfo()
{   
    Ogre::OverlayElement *oe;
    oe = _overlayManager->getOverlayElement("fpsInfo");
    oe->setCaption(Ogre::StringConverter::toString(_fps));
    oe = _overlayManager->getOverlayElement("camPosInfo");
    oe->setCaption(Ogre::StringConverter::toString(_camera->getPosition()));
    oe = _overlayManager->getOverlayElement("camRotInfo");
    oe->setCaption(Ogre::StringConverter::toString(_camera->getDirection()));
    oe = _overlayManager->getOverlayElement("modRotInfo");
    //Ogre::Quaternion q = _sceneMgr->getSceneNode("carGroupC1red")->getOrientation();
    oe->setCaption(//Ogre::String("RotZ: ") + 
                   //Ogre::StringConverter::toString(_car->getSceneNode()->getOrientation()) + 
                   Ogre::String("Vel: ") + Ogre::StringConverter::toString(_velocidad));
}

bool testStateVehicRayCast::frameEnded(const Ogre::FrameEvent &evt)
{
    return !_exitGame;
}

bool testStateVehicRayCast::keyPressed(const OIS::KeyEvent &e)
{

    if (e.key == OIS::KC_L)
    {    _freeCamera = !_freeCamera; cout << "Camara libre: " << _freeCamera << endl; }
    
    if (e.key == OIS::KC_C)
    {
        // Las enum class serán recomendables pero según que código queda horrible, vaya tela.
        _vista = static_cast<camara_view>(static_cast<int>(_vista)+1);
        _vista = static_cast<camara_view>(static_cast<int>(_vista) % static_cast<int>(camara_view::TOTAL_COUNT));
        colocaCamara();
    }    
    
    if (e.key == OIS::KC_V)
    {
        _cursorVehiculo += 1;
        _cursorVehiculo %= _vCarsRayCast.size();
    }    
    
/*    if (e.key == OIS::KC_R)
        _carRayCast->recolocar(_carRayCast->getPosicionActual());*/
    if (e.key == OIS::KC_R)
        _vCarsRayCast[_cursorVehiculo]->recolocar(_vCarsRayCast[_cursorVehiculo]->getPosicionActual(),Quaternion(1,0,0,0));
        
        
/*    if (e.key == OIS::KC_P)
        cout << "posicion actual coche raycast:" << _carRayCast->getPosicionActual() << endl;*/
    if (e.key == OIS::KC_P)
        cout << "posicion actual coche raycast:" << _vCarsRayCast[_cursorVehiculo]->getPosicionActual() << endl;

    flagKeys(true);
    
    return true;
}

void testStateVehicRayCast::flagKeys(bool flag)
{
    if (flag)
    {
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP))
            _keys |= static_cast<size_t>(keyPressed_flags::UP);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN))
            _keys |= static_cast<size_t>(keyPressed_flags::DOWN);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT))
            _keys |= static_cast<size_t>(keyPressed_flags::LEFT);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
            _keys |= static_cast<size_t>(keyPressed_flags::RIGHT);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_INSERT))
            _keys |= static_cast<size_t>(keyPressed_flags::INS);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DELETE))
            _keys |= static_cast<size_t>(keyPressed_flags::DEL);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGUP))
            _keys |= static_cast<size_t>(keyPressed_flags::PGUP);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGDOWN))
            _keys |= static_cast<size_t>(keyPressed_flags::PGDOWN);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD1))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD1);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD2))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD2);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD3))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD3);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD5))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD5);
    }
    else
    {
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP))
            _keys = ~(~_keys | static_cast<size_t>(keyPressed_flags::UP));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::DOWN));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::LEFT));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::RIGHT));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_INSERT))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::INS));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DELETE))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::DEL));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGUP))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::PGUP));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGDOWN))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::PGDOWN));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD1))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD1));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD2))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD2));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD3))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD3));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD5))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD5));
            
    }
}

bool testStateVehicRayCast::keyReleased(const OIS::KeyEvent &e)
{

    flagKeys(false);

    if (e.key == OIS::KC_ESCAPE)
        _exitGame = true;

    return true;
}

bool testStateVehicRayCast::mouseMoved(const OIS::MouseEvent &e)
{
    return true;
}

bool testStateVehicRayCast::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

bool testStateVehicRayCast::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

testStateVehicRayCast *testStateVehicRayCast::getSingletonPtr()
{
    return msSingleton;
}

testStateVehicRayCast &testStateVehicRayCast::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}

testStateVehicRayCast::~testStateVehicRayCast(){}

void testStateVehicRayCast::createLight()
{
//    //_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
//    _sceneMgr->setShadowTextureCount(2);
//    _sceneMgr->setShadowTextureSize(512);
//    Light *light = _sceneMgr->createLight("Light1");
//    light->setPosition(30, 30, 0);
//    light->setType(Light::LT_SPOTLIGHT);
//    light->setDirection(Vector3(-1, -1, 0));
//    light->setSpotlightInnerAngle(Degree(60.0f));
//    light->setSpotlightOuterAngle(Degree(80.0f));
//    light->setSpotlightFalloff(0.0f);
//    light->setCastShadows(true);

    //_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    _sceneMgr->setShadowTextureCount(2);
    _sceneMgr->setShadowTextureSize(512);
    
    std::vector< pair<Vector3,Vector3> > luces { pair<Vector3,Vector3>(Vector3(0,30,-30),Vector3(0,-1,0.5)),
                                                 pair<Vector3,Vector3>(Vector3(-50,30,0),Vector3(1,-1,0.5))
                                                 ,pair<Vector3,Vector3>(Vector3(50,30,0),Vector3(-1,-1,0.5)),
                                                 pair<Vector3,Vector3>(Vector3(0,30,30),Vector3(0,-1,-0.5)),
    };
    
//    Light *light = _sceneMgr->createLight("LightPlayState1");
//    //light->setPosition(30, 30, 0);
//    light->setPosition(0, 30, -30);
//    light->setType(Light::LT_SPOTLIGHT);
//    light->setDirection(Vector3(0, -1, 0.5));
//    light->setSpotlightInnerAngle(Degree(60.0f));
//    light->setSpotlightOuterAngle(Degree(80.0f));
//    light->setSpotlightFalloff(0.0f);
//    light->setCastShadows(true);
    
    for (size_t j=0; j<luces.size(); j++)
    {
        Light *light = _sceneMgr->createLight("Light" + std::to_string(j));
        //light->setPosition(30, 30, 0);
        light->setPosition(luces.at(j).first);
        light->setType(Light::LT_SPOTLIGHT);
        light->setDirection(luces.at(j).second);
        light->setSpotlightInnerAngle(Degree(60.0f));
        light->setSpotlightOuterAngle(Degree(80.0f));
        light->setSpotlightFalloff(0.0f);
        light->setCastShadows(true);
    }



}

void testStateVehicRayCast::createFloor() 
{
    SceneNode *floorNode = _sceneMgr->createSceneNode("floor");
    Plane planeFloor;
    planeFloor.normal = Vector3(0, 1, 0);
    planeFloor.d = 2;
    MeshManager::getSingleton().createPlane("FloorPlane", 
                                          ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                          planeFloor, 200000, 200000, 20, 20, true, 1, 9000, 9000, Vector3::UNIT_Z);
    Entity *entFloor = _sceneMgr->createEntity("floor", "FloorPlane");
    entFloor->setCastShadows(true);
    entFloor->setMaterialName("floor");
    floorNode->attachObject(entFloor);
    _sceneMgr->getRootSceneNode()->addChild(floorNode);
    _floorShape = new StaticPlaneCollisionShape(Ogre::Vector3(0, 1, 0), 0);
    // Con VehicleRaycast no se pueden usar máscaras de colisión, no funcionan y los coches no colisionan con nada.
//    _floorBody = new RigidBody("rigidBodyPlane", _world.get(), COL_FLOOR, COL_CAMERA | COL_CAR | COL_TRACK | COL_TRACK_COLISION);
    _floorBody = new RigidBody("rigidBodyPlane", _world.get());

    _floorBody->setStaticShape(_floorShape, 0.5, 0.8);
    floorNode->setPosition(Vector3(0, 2, 0));
}


void testStateVehicRayCast::createPlaneRoad()
{
//    nodoOgre_t nodoXML = SceneNodeConfig::getSingleton().getInfoNodoOgre("PlaneRoadBig");
//    SceneNode* planeRoadNode = _sceneMgr->createSceneNode(nodoXML.nombreNodo);
//    Entity* planeRoadEnt = _sceneMgr->createEntity(nodoXML.nombreEntidad,nodoXML.nombreMalla);
//    planeRoadEnt->setCastShadows(true);
//    planeRoadNode->attachObject(planeRoadEnt);
//    _sceneMgr->getRootSceneNode()->addChild(planeRoadNode);
//    
//   OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(planeRoadEnt);
//   OgreBulletCollisions::TriangleMeshCollisionShape *roadTrimesh = trimeshConverter->createTrimesh();
//   OgreBulletDynamics::RigidBody *planeRoadBody = new OgreBulletDynamics::RigidBody(nodoXML.nombreNodo, _world.get());
//   planeRoadBody->setShape(planeRoadNode, roadTrimesh, nodoXML.frictionBullet, nodoXML.bodyRestitutionBullet, nodoXML.masaBullet, nodoXML.posInicial);

    nodoOgre_t nodoXML = SceneNodeConfig::getSingleton().getInfoNodoOgre("PlaneRoadBig");
    SceneNode* _planeRoadNode = _sceneMgr->createSceneNode(nodoXML.nombreNodo);
    
    Plane planeRoad;
    planeRoad.normal = Vector3(0, 1, 0);
    planeRoad.d = 2;
    MeshManager::getSingleton().createPlane("PlaneRoad", 
                                          ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                          planeRoad, 300, 400, 20, 20, true, 1, 500, 500, Vector3::UNIT_X);
    Entity* planeRoadEnt = _sceneMgr->createEntity(nodoXML.nombreEntidad,"PlaneRoad");
    planeRoadEnt->setCastShadows(true);
    planeRoadEnt->setMaterialName("Asfalto");
    _planeRoadNode->attachObject(planeRoadEnt);
    _sceneMgr->getRootSceneNode()->addChild(_planeRoadNode);
    CollisionShape* planeRoadShape = new StaticPlaneCollisionShape(Ogre::Vector3(0, 1, 0), 0);
    OgreBulletDynamics::RigidBody *_planeRoadBody = new RigidBody(nodoXML.nombreNodo, _world.get());
    _planeRoadBody->setStaticShape(planeRoadShape, nodoXML.bodyRestitutionBullet,nodoXML.frictionBullet,Vector3(0, 2., 0));//nodoXML.posInicial);
    _planeRoadNode->setPosition(Vector3(0, 3.99, 0));
    _planeRoadBody->getBulletObject()->setUserPointer(new rigidBody_data(tipoRigidBody::CARRETERA,_planeRoadBody));
    
}

void testStateVehicRayCast::createScene()
{
  
    _sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    _sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
    _sceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
    _sceneMgr->setShadowFarDistance(100);
    _sceneMgr->setSkyBox(true, "skybox");

    createOverlay();
    createLight();
    createFloor();
    //createMyGui();
    
    _track = unique_ptr<track>(new track("track1NoRoadBig",_world.get(),Vector3(0,0,0),_sceneMgr));
    createPlaneRoad();
    _car = unique_ptr<car>(new car("carGroupC1",_world.get(),SceneNodeConfig::getSingleton().getInfoNodoOgre("carGroupC1").posInicial,_sceneMgr,"",nullptr));
    
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("kart",Vector3(-4,2.1,-22),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("farara-sport",Vector3(-2,2.1,-22),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("formula",Vector3(-6,2.1,-22),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("groupC1",Vector3(0,2.1,-22),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("groupC2",Vector3(2,2.1,-22),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("lamba-sport",Vector3(4,2.1,-22),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("parsche-sport",Vector3(6,2.1,-22),_sceneMgr,_world.get())));
    
    for (auto it = _vCarsRayCast.begin(); it != _vCarsRayCast.end(); ++it)
        (*it)->buildVehiculo();
    
    _cursorVehiculo = 0;
    
    // Carga de la malla que bordea el circuito para que no se salga el coche, SOLO PARA PRUEBAS
//    nodoOgre_t nodoConfigCol = _scn.getInfoNodoOgre("track1colLateral");
//    Entity* entColLateral = _sceneMgr->createEntity(nodoConfigCol.nombreEntidad, nodoConfigCol.nombreMalla);
//    SceneNode* nodoColLateral = _sceneMgr->createSceneNode(nodoConfigCol.nombreNodo);
//    nodoColLateral->attachObject(entColLateral);
//    _track->getSceneNode()->addChild(nodoColLateral);
//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entColLateral);
//    OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter->createTrimesh();
//    OgreBulletDynamics::RigidBody* body = new OgreBulletDynamics::RigidBody(nodoConfigCol.nombreNodo, _world.get(), COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
//    body->setShape(nodoColLateral,tri,nodoConfigCol.bodyRestitutionBullet,nodoConfigCol.frictionBullet,nodoConfigCol.masaBullet,nodoConfigCol.posShapeBullet);
//    nodoColLateral->setVisible(false);

    // Prueba de bullet para un CheckPoint
    nodoOgre_t infoCheckPoint = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("CheckPointPlane");
    Entity* entCheckPoint = _sceneMgr->createEntity(infoCheckPoint.nombreEntidad, infoCheckPoint.nombreMalla);
    SceneNode* nodoCheckPoint = _sceneMgr->createSceneNode(infoCheckPoint.nombreNodo);
    nodoCheckPoint->attachObject(entCheckPoint);
    _sceneMgr->getRootSceneNode()->addChild(nodoCheckPoint);
    Vector3 aux = _vCarsRayCast[_cursorVehiculo]->getPosicionActual();
    nodoCheckPoint->setPosition(Vector3(aux.x - 10, 0,aux.z));

    
//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entCheckPoint);
    OgreBulletCollisions::BoxCollisionShape* boxShape = new BoxCollisionShape(entCheckPoint->getBoundingBox().getHalfSize());
//    OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter->createTrimesh();
//    ConvexHullCollisionShape* convexShape = trimeshConverter->createConvex();
//    convexShape->getBulletShape()->setMargin(-0.05);

    _bodyCheckPoint = new OgreBulletDynamics::RigidBody(infoCheckPoint.nombreNodo, _world.get());//,btCollisionObject::CF_NO_CONTACT_RESPONSE);
    //_bodyCheckPoint->setShape(nodoCheckPoint,tri,infoCheckPoint.bodyRestitutionBullet,infoCheckPoint.frictionBullet,infoCheckPoint.masaBullet,infoCheckPoint.posShapeBullet);
    _bodyCheckPoint->setShape(nodoCheckPoint,boxShape,infoCheckPoint.bodyRestitutionBullet,infoCheckPoint.frictionBullet,infoCheckPoint.masaBullet,Vector3(aux.x - 10, 0,aux.z));
    _bodyCheckPoint->getBulletObject()->setUserPointer(new datosVarios("checkPoint de prueba",1));
    

//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(_ent);
//    _convexShape = trimeshConverter->createConvex();
//    _convexShape->getBulletShape()->setMargin(-0.05);
//    _body->setStaticShape(_convexShape, 1, 1, nodoConfig.posShapeBullet, Quaternion::IDENTITY);


    
    
    nodoCheckPoint->setVisible(true);
    
  
}

void testStateVehicRayCast::createOverlay() 
{
//    _sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());  
//    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();    
    
    _overlayManager = Ogre::OverlayManager::getSingletonPtr();
    Ogre::Overlay *overlay = _overlayManager->getByName("Info");
    overlay->show();
}

void testStateVehicRayCast::cargarParametros(string archivo, bool consoleOut)
{
    //Cargamos la info para generar elementos de la escena (scenenodes, cameras, lights)
    SceneNodeConfig::getSingleton().load_xml("SceneNodes.xml");
    
    map_nodos_t nodos = SceneNodeConfig::getSingleton().getMapNodos();
    map_cameras_t camaras = SceneNodeConfig::getSingleton().getMapCameras();
    map_vehiculos_ray_cast_t vehicRayCast = SceneNodeConfig::getSingleton().getMapVehiculosRaycast();
    
    if (consoleOut)
    {
        for (it_map_nodos it = nodos.begin(); it != nodos.end(); ++it)
                // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
                // first sería la clave y second el valor.
                cout << (*it).second << endl;
                
        for (it_map_cameras it = camaras.begin(); it != camaras.end(); ++it)
                // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
                // first sería la clave y second el valor.
                cout << (*it).second << endl;
                
        for (it_map_vehiculos_ray_cast it = vehicRayCast.begin(); it != vehicRayCast.end(); ++it)
            cout << (*it).second << endl;
    }
}

void testStateVehicRayCast::configurarCamaraPrincipal()
{
    //Configuramos la camara
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    nodoCamera_t cam = SceneNodeConfig::getSingleton().getInfoCamera("IntroCamera");
    nodoVehiculoRayCast_t aux = SceneNodeConfig::getSingleton().getInfoVehiculoRayCast("kart"); // ESTO HAY QUE CAMBIARLO QUE EL LOOKAT SEA SIEMPRE AL PLAYER HUMANO.
    _camera->setAspectRatio(width / height);
    _camera->setPosition(cam.posInicial);
    //_camera->lookAt(cam.lookAt);
    _camera->lookAt(aux.posicion);
    _camera->setNearClipDistance(cam.nearClipDistance);
    _camera->setFarClipDistance(cam.farClipDistance);
    _nodoVista = _sceneMgr->createSceneNode("nodoVista");
    cout << "padre nodovista: " << _nodoVista->getParent() << endl;
    cout << "padre _camara: " << _camera->getParentNode() << endl;
}

void testStateVehicRayCast::initBulletWorld(bool showDebug)
{
    _debugDrawer = new OgreBulletCollisions::DebugDrawer();
    _debugDrawer->setDrawWireframe(true);
    SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("debugNode", Vector3::ZERO);
    node->attachObject(static_cast<SimpleRenderable *>(_debugDrawer));
    
//    AxisAlignedBox boundBox = AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000),Ogre::Vector3(10000, 10000, 10000));
    AxisAlignedBox boundBox = AxisAlignedBox(Ogre::Vector3(-100, -100, -100),Ogre::Vector3(100, 100, 100));
    _world = shared_ptr<OgreBulletDynamics::DynamicsWorld>(new DynamicsWorld(_sceneMgr, boundBox, Vector3(0, -9.8, 0))); //, true, true, 15000));
    _world.get()->setDebugDrawer(_debugDrawer);
    _world.get()->setShowDebugShapes(showDebug);
}

void testStateVehicRayCast::destroyMyGui()
{
 MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void testStateVehicRayCast::createMyGui()
{
    MyGUI::OgrePlatform *mp = new MyGUI::OgrePlatform();
    mp->initialise(_root->getAutoCreatedWindow(), Ogre::Root::getSingleton().getSceneManager("SceneManager"));
    MyGUI::Gui *mGUI = new MyGUI::Gui();
    mGUI->initialise();
    layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_test.layout");
//    MyGUI::PointerManager::getInstancePtr()->setVisible(true);
}


//void testStateVehicRayCast::handleCollision(btCollisionObject *body0, btCollisionObject *body1)
//{
//
//    if (body0 == _bodyCheckPoint->getBulletRigidBody() || body1 == _bodyCheckPoint->getBulletRigidBody()) 
//    {
//        cout << "colision con el CheckPoint " << endl;
//        btCollisionObject* checkBody = (body0 == _bodyCheckPoint->getBulletRigidBody()) ? body0 : body1;
//        btCollisionObject* otherObject = (checkBody == body0)?body1:body0;
//        // Chequeamos con quien ha colisionado el plano
//        for (std::vector< std::unique_ptr<CarRayCast> >::iterator it = _vCarsRayCast.begin();  it != _vCarsRayCast.end(); ++it) 
//        {
//            if ( (btCollisionObject*) (*it)->getRigidBody() == otherObject) 
//            {           
//            }
//        }
//    
//    }
//    
//}

bool testStateVehicRayCast::compruebaCheckPoint()
{
    static size_t i = 0;
    
    //https://youtu.be/nyJa-WKmWqE GRACIAS A DIOS ENCONTRÉ A ESTE CRACK. Documentación Bullet = BigShit!
    btVector3 inicio = convert(_vCarsRayCast[_cursorVehiculo]->getPosicionActual());
    //btVector3 fin = convert(_vCarsRayCast[_cursorVehiculo]->getPosicionActual() * Ogre::Vector3::NEGATIVE_UNIT_Y * 10); 
    btVector3 fin = inicio;
    fin.setY(fin.getY() -10);
    
    // VAMOS A DIBUJAR EL RAYO A VER QUE COÑO ESTA HACIENDO    
//    ManualObject* manual = _sceneMgr->createManualObject("rayo_" + to_string(i));
//    manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
//        manual->position(convert(inicio)); //start
//        manual->position(convert(fin));    //end
//    manual->end();
//    i++;
//    _sceneMgr->getRootSceneNode()->attachObject(manual);    
    
    btCollisionWorld::AllHitsRayResultCallback rayCallback(inicio,fin);
                                                           
    _world->getBulletDynamicsWorld()->rayTest(inicio,fin, rayCallback);
    
    if (rayCallback.hasHit())
    {
        cout << "El rayo ha impactado con " << rayCallback.m_collisionObjects.size() << " objetos." << endl;
        for (int i=0; i<rayCallback.m_collisionObjects.size(); i++)
        {
            if (rayCallback.m_collisionObjects[i]->getUserPointer())
            {
                //cout << "Datos varios NOMBRE: " << ((datosVarios*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->nombre << endl;
                //cout << "Datos varios ID: " << ((datosVarios*)(rayCallback.m_collisionObjects[i]->getUserPointer()))->id << endl;
                cout << "rayo" << endl;
            }
        }
        return true;
    }
    
    return false;
    
}

bool testStateVehicRayCast::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool testStateVehicRayCast::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool testStateVehicRayCast::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}


