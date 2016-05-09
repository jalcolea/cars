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

    //Configuramos la camara
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    _camera->setAspectRatio(width / height);
    _camera->setPosition(Vector3(0, 12, 32));
    _camera->lookAt(_sceneMgr->getRootSceneNode()->getPosition());
    _camera->lookAt(0,0,0);
    _camera->setNearClipDistance(0.1);
    _camera->setFarClipDistance(100);

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
  return !_exitGame;
}

bool testState::frameEnded(const Ogre::FrameEvent &evt)
{
    return !_exitGame;
}

bool testState::keyPressed(const OIS::KeyEvent &e)
{

    // Transición al siguiente estado.
    // Espacio --> PlayState
    if (e.key == OIS::KC_SPACE)
    {
        changeState(MenuState::getSingletonPtr());
        sounds::getInstance()->play_effect("push");
    }
//    else if (e.key == OIS::KC_T)
//    {
//        changeState(testwiimoteState::getSingletonPtr());
//        
//    }
    
    return true;

}

bool testState::keyReleased(const OIS::KeyEvent &e)
{
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


void testState::createScene()
{
  createLight();
  //createMyGui();
  
  _scn.load_xml("SceneNodes.xml");
  nodoOgre_t nodo = _scn.getInfoNodoOgre("track1");
  
  SceneNode* nodoTrack1 = _sceneMgr->createSceneNode(nodo.nombreNodo);
  Entity* entTrack1 = _sceneMgr->createEntity(nodo.nombreEntidad,nodo.nombreMalla);
  nodoTrack1->attachObject(entTrack1);
  _sceneMgr->getRootSceneNode()->addChild(nodoTrack1);
  
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


