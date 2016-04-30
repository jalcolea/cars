#include "LooseState.h"
#include "IntroState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "records.h"
#include "constants.h"

#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>

#define TIME_RESUCITANDO 3

template<> LooseState* Ogre::Singleton<LooseState>::msSingleton = 0;

void LooseState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  createScene();
  _exitGame = false;
}

void LooseState::exit()
{
 _root->getAutoCreatedWindow()->removeAllViewports();
  destroyMyGui();
}

void LooseState::pause()
{
}

void LooseState::resume()
{
}

bool LooseState::frameStarted(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  return true;
}

bool LooseState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}

void LooseState::save_record()
{
        //records::getInstance()->add_record(user_name_txt->getCaption(),get_score());
        records::getInstance()->add_record(user_name_txt->getCaption(),points);
        records::getInstance()->saveFile(NULL);
        //sounds::getInstance()->play_effect("eat_ghost");
        //user_name_txt->setVisible(false);
}

bool LooseState::keyPressed(const OIS::KeyEvent &e) 
{
     MyGUI::UString txt = user_name_txt->getCaption();
    if ((int)e.key==14 && txt.size()>0)
{
txt.resize(txt.size()-1);
}
    else
    {
      if (((int)e.text >=65 && (int)e.text<=90) || ((int)e.text>=97 && (int)e.text<=122))
      {
        if (txt.size()<CONSTANTS_MAX_USERNAME_SIZE) txt.push_back(e.text);
      }
    }
    user_name_txt->setCaption(txt);
    if (e.key==OIS::KC_RETURN)
    {
        cout << "NEW RECORD TO SAVE" << endl;
        save_record();
        popState();
    }

 
  return true;
}

bool LooseState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool LooseState::mouseMoved(const OIS::MouseEvent &e)
{

  return true;
}

bool LooseState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  if (btn_resume->_checkPoint(x,y))
  {
    save_record();
    sounds::getInstance()->play_effect("push");
    popState();
  }
  return true;
}

bool LooseState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

LooseState* LooseState::getSingletonPtr ()
{
    return msSingleton;
}

LooseState& LooseState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

void LooseState::destroyMyGui()
{
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void LooseState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_loose.layout");
  btn_resume = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_resume");
  user_name_txt = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("user_name");

}

void LooseState::createScene()
{
 createMyGui();
}

bool LooseState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool LooseState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool LooseState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}
