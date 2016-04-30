#include "ControlsState.h"
#include "RecordsState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "records.h"

#include "OgreTextAreaOverlayElement.h"
#include "OgreStringConverter.h"
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

template<> RecordsState* Ogre::Singleton<RecordsState>::msSingleton = 0;

using namespace std;
using namespace Ogre;

void RecordsState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  createScene();
  loadRecords();
  _exitGame = false;
}

void RecordsState::loadRecords()
{
          int result = 0;
          int cont=0;
          string name;
          int points;
          string tmp_users="";
          string tmp_points="";
          char tmp_char [64];
          int max_records = 10;
          result = records::getInstance()->getNext(name,points,true);
          cont++;
          while (result == 0)
          {

            if (result == 0)
            {
              sprintf(tmp_char,"\%s\n",name.c_str());
              tmp_users += string(tmp_char);
              sprintf(tmp_char,"\%d\n",points);
              tmp_points += string(tmp_char);
            }
            if (max_records!=0 && cont >= max_records) break;
            result = records::getInstance()->getNext(name,points);
            cont++;
          }
          score_names_txt->setCaption(tmp_users);
          score_points_txt->setCaption(tmp_points);
}
void RecordsState::exit ()
{
 //_sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();

  destroyMyGui();
}

void RecordsState::pause()
{
}

void RecordsState::resume()
{
}

bool RecordsState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool RecordsState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}


bool RecordsState::keyPressed(const OIS::KeyEvent &e)
{
  popState();
  
  return true;
}

bool RecordsState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool RecordsState::mouseMoved(const OIS::MouseEvent &e)
{
  return true;
}

bool RecordsState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
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

bool RecordsState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

RecordsState* RecordsState::getSingletonPtr()
{
    return msSingleton;
}

RecordsState& RecordsState::getSingleton()
{ 
  assert(msSingleton);
  return *msSingleton;
}

RecordsState::~RecordsState()
{
}


void RecordsState::createScene()
{
  createMyGui();
}

void RecordsState::destroyMyGui()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void RecordsState::createMyGui()
{
//  string name="";
//  char points_str [32];
//  int points=0;
  layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_records.layout");
  btn_back = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_back");
//  btn_back->eventMouseButtonClick = MyGUI::newDelegate(this, &RecordsState::notifyButtonPress);
//  high_score_txt = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("high_score");
//  score_positions_txt = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("score_positions");
  score_points_txt = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("score_points");
  score_names_txt = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("score_names");
//  records::getInstance()->getBest(name,points);
//  sprintf(points_str,"%d",points);
//  high_score_txt->setCaption(points_str);
}

bool RecordsState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool RecordsState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool RecordsState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}

void RecordsState::notifyButtonPress(MyGUI::Widget* _widget)
{
  string name;
  name = _widget->getName();

  if (name=="btn_back") popState();
}
