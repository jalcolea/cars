#include "ControlsState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "records.h"

template<> ControlsState* Ogre::Singleton<ControlsState>::msSingleton = 0;

using namespace std;
using namespace Ogre;

void ControlsState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  createScene();
  _exitGame = false;
}

void ControlsState::exit ()
{
 //_sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();

  destroyMyGui();
}

void ControlsState::pause()
{
}

void ControlsState::resume()
{
}

bool ControlsState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool ControlsState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}


bool ControlsState::keyPressed(const OIS::KeyEvent &e)
{
  popState();
  
  return true;
}

bool ControlsState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool ControlsState::mouseMoved(const OIS::MouseEvent &e)
{
  return true;
}

bool ControlsState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  if (btn_back->_checkPoint(x,y))
  {
    sounds::getInstance()->play_effect("push");
    popState();
  }
  else if (btn_mouse->_checkPoint(x,y)) 
  {
    sounds::getInstance()->play_effect("push");
    text->setCaption("MOUSE Modo de control recomendado");
  }
  else if (btn_wiimote->_checkPoint(x,y)) 
  {
    sounds::getInstance()->play_effect("push");
    text->setCaption("WIIMOTE Disponible solo en modo desarrollador");
  }
  else text->setCaption("");
  return true;
}

bool ControlsState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

ControlsState* ControlsState::getSingletonPtr()
{
    return msSingleton;
}

ControlsState& ControlsState::getSingleton()
{ 
  assert(msSingleton);
  return *msSingleton;
}

ControlsState::~ControlsState()
{
}


void ControlsState::createScene()
{
  createMyGui();
}

void ControlsState::destroyMyGui()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

string ControlsState::get_high_score()
{
  string recordname;
  int recordpoints;
  char msg [128];
  records::getInstance()->getBest (recordname,recordpoints);
  sprintf (msg,"%s %d",recordname.c_str(),recordpoints);
  return string(msg);
}

void ControlsState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_controls.layout");
  btn_back = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_back");
  btn_mouse = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_mouse");
  btn_wiimote = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_wiimote");
  text = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("text");
  edt_high = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("edt_high");
  edt_high->setCaption(get_high_score());
}

bool ControlsState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool ControlsState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool ControlsState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}
