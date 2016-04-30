#include "ControlsState.h"
#include "CreditsState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "records.h"

template<> CreditsState* Ogre::Singleton<CreditsState>::msSingleton = 0;

using namespace std;
using namespace Ogre;

void CreditsState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  createScene();
  _exitGame = false;
}

void CreditsState::exit ()
{
 //_sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();

  destroyMyGui();
}

void CreditsState::pause()
{
}

void CreditsState::resume()
{
}

bool CreditsState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool CreditsState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}


bool CreditsState::keyPressed(const OIS::KeyEvent &e)
{
  popState();
  
  return true;
}

bool CreditsState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool CreditsState::mouseMoved(const OIS::MouseEvent &e)
{
  return true;
}

bool CreditsState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  if (btn_back->_checkPoint(x,y))
  {
    sounds::getInstance()->play_effect("push");
    popState();
  }
  return true;
}

bool CreditsState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

CreditsState* CreditsState::getSingletonPtr()
{
    return msSingleton;
}

CreditsState& CreditsState::getSingleton()
{ 
  assert(msSingleton);
  return *msSingleton;
}

CreditsState::~CreditsState()
{
}


void CreditsState::createScene()
{
  createMyGui();
}

void CreditsState::destroyMyGui()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

string CreditsState::get_high_score()
{
  string recordname;
  int recordpoints;
  char msg [128];
  records::getInstance()->getBest (recordname,recordpoints);
  sprintf (msg,"%s %d",recordname.c_str(),recordpoints);
  return string(msg);
}

void CreditsState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_credits.layout");
  btn_back = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_back");
  btn_back->eventMouseButtonClick = MyGUI::newDelegate(this, &CreditsState::notifyButtonPress);
  edt_high = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("edt_high");
  edt_high->setCaption(get_high_score());
}

bool CreditsState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool CreditsState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool CreditsState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}

void CreditsState::notifyButtonPress(MyGUI::Widget* _widget)
{
  string name;
  name = _widget->getName();

  if (name=="btn_backº") popState();
}
