#include <iostream>
#include "testState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

using namespace std;
using namespace Ogre;

template<> testState *Ogre::Singleton<testState>::msSingleton = 0;

void testState::enter()
{
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


    //El fondo del pacman siempre es negro
    _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));

    //Cargamos la info para generar elementos de la escena (scenenodes, cameras, lights)
    _scn.load_xml("SceneNodes.xml");
    
    map_nodos_t nodos = _scn.getMapNodos();
    map_cameras_t camaras = _scn.getMapCameras();
    for (it_map_nodos it = nodos.begin(); it != nodos.end(); ++it)
            // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
            // first sería la clave y second el valor.
            cout << (*it).second << endl;
            
    for (it_map_cameras it = camaras.begin(); it != camaras.end(); ++it)
            // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
            // first sería la clave y second el valor.
            cout << (*it).second << endl;


    //Configuramos la camara
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    nodoCamera_t cam = _scn.getInfoCamera("IntroCamera");
    _camera->setAspectRatio(width / height);
    _camera->setPosition(cam.posInicial);
    _camera->lookAt(cam.lookAt);
    _camera->setNearClipDistance(cam.nearClipDistance);
    _camera->setFarClipDistance(cam.farClipDistance);

    createScene();
    
    _exitGame = false;
    _deltaT = 0;
    sounds::getInstance()->play_music("begin");

}

void testState::exit()
{
//    sounds::getInstance()->halt_music();
//    destroyMyGui();
    _sceneMgr->clearScene();
    _root->getAutoCreatedWindow()->removeAllViewports();
}

void testState::pause()
{
}

void testState::resume()
{

}

bool testState::frameStarted(const Ogre::FrameEvent &evt)
{
    _deltaT = evt.timeSinceLastFrame;
    int fps = 1.0 / _deltaT;


    _camera->moveRelative(_vt * _deltaT * 20.0 /*tSpeed*/);
    if (_camera->getPosition().length() < 10.0) 
        _camera->moveRelative(-_vt * _deltaT * 20.0 /*tSpeed*/);

    Ogre::OverlayElement *oe;
    oe = _overlayManager->getOverlayElement("fpsInfo");
    oe->setCaption(Ogre::StringConverter::toString(fps));
    oe = _overlayManager->getOverlayElement("camPosInfo");
    oe->setCaption(Ogre::StringConverter::toString(_camera->getPosition()));
    oe = _overlayManager->getOverlayElement("camRotInfo");
    oe->setCaption(Ogre::StringConverter::toString(_camera->getDirection()));
    oe = _overlayManager->getOverlayElement("modRotInfo");
    Ogre::Quaternion q = _sceneMgr->getSceneNode("carKartWhite")->getOrientation();
    oe->setCaption(Ogre::String("RotZ: ") + Ogre::StringConverter::toString(q.getYaw()));

    return !_exitGame;

}

bool testState::frameEnded(const Ogre::FrameEvent &evt)
{
    return !_exitGame;
}

bool testState::keyPressed(const OIS::KeyEvent &e)
{
    _vt = Ogre::Vector3(0,0,0);

    if (e.key == OIS::KC_SPACE)
    {
        changeState(MenuState::getSingletonPtr());
        sounds::getInstance()->play_effect("push");
    }

    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP)) _vt+= Ogre::Vector3(0,0,-1);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN)) _vt+= Ogre::Vector3(0,0,1);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT)) _vt+= Ogre::Vector3(-1,0,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) _vt+= Ogre::Vector3(1,0,0);

/*
    if (e.key == OIS::KC_UP) _vt += Ogre::Vector3(0,0,-1);
    if (e.key == OIS::KC_DOWN) _vt += Ogre::Vector3(0,0,1);
    if (e.key == OIS::KC_LEFT) _vt += Ogre::Vector3(-1,0,0);
    if (e.key == OIS::KC_RIGHT) _vt += Ogre::Vector3(1,0,0);
*/    
    
    
    return true;

}

bool testState::keyReleased(const OIS::KeyEvent &e)
{
    _vt = Ogre::Vector3(0,0,0);

    if (e.key == OIS::KC_ESCAPE)
    {
        _exitGame = true;
    }
    return true;
}

bool testState::mouseMoved(const OIS::MouseEvent &e)
{
    return true;
}

bool testState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

bool testState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

testState *testState::getSingletonPtr()
{
    return msSingleton;
}

testState &testState::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}

testState::~testState()
{
}

void testState::createLight()
{
    _sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    _sceneMgr->setShadowTextureCount(2);
    _sceneMgr->setShadowTextureSize(512);
    Light *light = _sceneMgr->createLight("Light1");
    light->setPosition(30, 30, 0);
    light->setType(Light::LT_SPOTLIGHT);
    light->setDirection(Vector3(-1, -1, 0));
    light->setSpotlightInnerAngle(Degree(60.0f));
    light->setSpotlightOuterAngle(Degree(80.0f));
    light->setSpotlightFalloff(0.0f);
    light->setCastShadows(true);
}

void testState::createFloor() 
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

//  _floorShape = new StaticPlaneCollisionShape(Ogre::Vector3(0, 1, 0), 0);
//
//  _floorBody = new RigidBody("rigidBodyPlane", _world.get(), COL_FLOOR,
//                             COL_CAMERA | COL_ACTIVATOR);
//  _floorBody->setStaticShape(_floorShape, 0.5, 0.8);
    floorNode->setPosition(Vector3(0, 2, 0));
//  btCollisionShape *floorShape = _floorShape->getBulletShape();

}


void testState::createScene()
{
  createOverlay();
  createLight();
  //createMyGui();
  

  nodoOgre_t nodo = _scn.getInfoNodoOgre("track1");
  
  SceneNode* nodoTrack1 = _sceneMgr->createSceneNode(nodo.nombreNodo);
  Entity* entTrack1 = _sceneMgr->createEntity(nodo.nombreEntidad,nodo.nombreMalla);
  nodoTrack1->attachObject(entTrack1);
  _sceneMgr->getRootSceneNode()->addChild(nodoTrack1);
  
  nodo = _scn.getInfoNodoOgre("carKartWhite");
  SceneNode* nodoKartWhite = _sceneMgr->createSceneNode(nodo.nombreNodo);
  Entity* entKartWhite = _sceneMgr->createEntity(nodo.nombreEntidad,nodo.nombreMalla);
  nodoKartWhite->attachObject(entKartWhite);
  nodoTrack1->addChild(nodoKartWhite);
  nodoKartWhite->setPosition(nodo.posInicial);
  nodoKartWhite->setOrientation(nodo.orientacion);
  
  //_camera->setAutoTracking(true,nodoKartWhite);
  
  
}

void testState::createOverlay() 
{
//    _sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());  
//    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();    
    
    _overlayManager = Ogre::OverlayManager::getSingletonPtr();
    Ogre::Overlay *overlay = _overlayManager->getByName("Info");
    overlay->show();
}

void testState::destroyMyGui()
{
 MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void testState::createMyGui()
{
    MyGUI::OgrePlatform *mp = new MyGUI::OgrePlatform();
    mp->initialise(_root->getAutoCreatedWindow(), Ogre::Root::getSingleton().getSceneManager("SceneManager"));
    MyGUI::Gui *mGUI = new MyGUI::Gui();
    mGUI->initialise();
    layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_test.layout");
//    MyGUI::PointerManager::getInstancePtr()->setVisible(true);
}

bool testState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool testState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool testState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}


