#include "PauseState.h"
#include "IntroState.h"
#include "MenuState.h"
#include "PlayState.h"

#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>

#define TIME_RESUCITANDO 3

template<> PauseState* Ogre::Singleton<PauseState>::msSingleton = 0;

void PauseState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  createScene();
  _exitGame = false;
}

void PauseState::exit()
{
 _root->getAutoCreatedWindow()->removeAllViewports();
  destroyMyGui();
}

void PauseState::pause()
{
}

void PauseState::resume()
{
}

bool PauseState::frameStarted(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  return true;
}

bool PauseState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}

bool PauseState::keyPressed(const OIS::KeyEvent &e) 
{
  // Tecla p --> Estado anterior.
      if (e.key == OIS::KC_P)  // Con  P otra vez reanudamos el PlayState
      {
        popState();
      }
  
  return true;
}

bool PauseState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool PauseState::mouseMoved(const OIS::MouseEvent &e)
{

  return true;
}

bool PauseState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  if (btn_resume->_checkPoint(x,y))
  {
    sounds::getInstance()->play_effect("push");
    popState();
  }
  return true;
}

bool PauseState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

PauseState* PauseState::getSingletonPtr ()
{
    return msSingleton;
}

PauseState& PauseState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

void PauseState::destroyMyGui()
{
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void PauseState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_pause.layout");
  btn_resume = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_resume");

}

void PauseState::createScene()
{
 createMyGui();
}

bool PauseState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool PauseState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool PauseState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}
