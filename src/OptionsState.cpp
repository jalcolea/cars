#include "OptionsState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "records.h"

template<> OptionsState* Ogre::Singleton<OptionsState>::msSingleton = 0;

using namespace std;
using namespace Ogre;

void OptionsState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  createScene();
  _exitGame = false;
}

void OptionsState::exit ()
{
 //_sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();

  destroyMyGui();
}

void OptionsState::pause()
{
}

void OptionsState::resume()
{
}

bool OptionsState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool OptionsState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}


bool OptionsState::keyPressed(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_S) {
    sounds::getInstance()->play_effect("push");
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    pushState(PlayState::getSingletonPtr());
  }
  else
{
  sounds::getInstance()->play_effect("push");
  popState();
}
  
  return true;
}

bool OptionsState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool OptionsState::mouseMoved(const OIS::MouseEvent &e)
{
  return true;
}

bool OptionsState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  if (btn_back->_checkPoint(x,y))
  {
    sounds::getInstance()->play_effect("push");
    popState();
  }
  /*else if (btn_mouse->_checkPoint(x,y)) 
  {
    sounds::getInstance()->play_effect("push");
    text->setCaption("MOUSE Modo de control recomendado");
  }
  else if (btn_wiimote->_checkPoint(x,y)) 
  {
    sounds::getInstance()->play_effect("push");
    text->setCaption("WIIMOTE Disponible solo en modo desarrollador");
  }
*/
  else text->setCaption("");
  return true;
}

bool OptionsState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

OptionsState* OptionsState::getSingletonPtr()
{
    return msSingleton;
}

OptionsState& OptionsState::getSingleton()
{ 
  assert(msSingleton);
  return *msSingleton;
}

OptionsState::~OptionsState()
{
}


void OptionsState::createScene()
{
  createMyGui();
}

void OptionsState::destroyMyGui()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

string OptionsState::get_high_score()
{
  string recordname;
  int recordpoints;
  char msg [128];
  records::getInstance()->getBest (recordname,recordpoints);
  sprintf (msg,"%s %d",recordname.c_str(),recordpoints);
  return string(msg);
}

void OptionsState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout(LAYOUT_OPTIONS);
  btn_back = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_back");
  //btn_mouse = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_mouse");
  //btn_wiimote = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_wiimote");
  text = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("text");
  edt_high = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("edt_high");
  edt_high->setCaption(get_high_score());
}

bool OptionsState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool OptionsState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool OptionsState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}
