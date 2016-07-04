#include <iostream>
#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "testStateVehicRayCast.h"
#include "carSelectorState.h"
#include "pathDrawerState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

using namespace std;
using namespace Ogre;

template<> IntroState *Ogre::Singleton<IntroState>::msSingleton = 0;

void IntroState::enter()
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
        cout << "IntroCamera no existe, creándola \n";
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
    _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

    //Configuramos la camara
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    _camera->setAspectRatio(width / height);
    //_camera->setPosition(Vector3(0, 12, 18));
    _camera->setPosition(Vector3(0, 0, 18));
    _camera->lookAt(_sceneMgr->getRootSceneNode()->getPosition());
    _camera->lookAt(0,0,0);
    _camera->setNearClipDistance(0.1);
    _camera->setFarClipDistance(100);

    createScene();
    _exitGame = false;
    _deltaT = 0;
    _timeIntroSound = 0;
    _introSoundDone = false;
    sounds::getInstance()->play_effect("intro");

}

void IntroState::exit()
{
    //sounds::getInstance()->halt_music();
    destroyMyGui();
    _sceneMgr->clearScene();
    _root->getAutoCreatedWindow()->removeAllViewports();
    if (!sounds::getInstance()->playing_music())
        sounds::getInstance()->play_music("mainTheme",-1);  // -1 loop forever :D
}

void IntroState::pause()
{
}

void IntroState::resume()
{

}

bool IntroState::frameStarted(const Ogre::FrameEvent &evt)
{
  _timeIntroSound += evt.timeSinceLastFrame;
  
  if (_timeIntroSound >= TIME_INTRO_SOUND && !_introSoundDone)
  {
      _introSoundDone = true;
      if (!sounds::getInstance()->playing_music())
        sounds::getInstance()->play_music("mainTheme",-1); // -1 loop forever :D
  }
  
  if (_timeIntroSound >= TIME_INTRO_DONE)
    changeState(MenuState::getSingletonPtr());

  
  
  
  return true;
}

bool IntroState::frameEnded(const Ogre::FrameEvent &evt)
{
    return true;
}

bool IntroState::keyPressed(const OIS::KeyEvent &e)
{

    // Transición al siguiente estado.
    // Espacio --> PlayState
    if (e.key == OIS::KC_SPACE || e.key == OIS::KC_RETURN)
    {
        changeState(MenuState::getSingletonPtr());
        sounds::getInstance()->play_effect("push");
    }
    else if (e.key == OIS::KC_T)
    {
        changeState(testStateVehicRayCast::getSingletonPtr());
        
    }
    else if (e.key == OIS::KC_I)
    {
        changeState(pathDrawerState::getSingletonPtr());
    }
    
    return true;

}

bool IntroState::keyReleased(const OIS::KeyEvent &e)
{
    if (e.key == OIS::KC_ESCAPE)
    {

        _exitGame = true;
    }
    return true;
}

bool IntroState::mouseMoved(const OIS::MouseEvent &e)
{
    return true;
}

bool IntroState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    changeState(MenuState::getSingletonPtr());
    sounds::getInstance()->play_effect("push");
    return true;
}

bool IntroState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

IntroState *IntroState::getSingletonPtr()
{
    return msSingleton;
}

IntroState &IntroState::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}

IntroState::~IntroState()
{
}


void IntroState::createScene()
{
  createMyGui();
}

void IntroState::destroyMyGui()
{
 MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}


void IntroState::createMyGui()
{
    MyGUI::OgrePlatform *mp = new MyGUI::OgrePlatform();
    mp->initialise(_root->getAutoCreatedWindow(), Ogre::Root::getSingleton().getSceneManager("SceneManager"));
    MyGUI::Gui *mGUI = new MyGUI::Gui();
    mGUI->initialise();
   layout = MyGUI::LayoutManager::getInstance().loadLayout(LAYOUT_INTRO);
  //MyGUI::PointerManager::getInstancePtr()->setVisible(true);
}

bool IntroState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool IntroState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool IntroState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}


