
#include "MenuState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "OptionsState.h"
#include "ControlsState.h"
#include "RecordsState.h"
#include "CreditsState.h"
#include "PauseState.h"
#include "WinState.h"
#include "LooseState.h"
#include "records.h"
#include "Carrusel.h"
#include "PlayWidget.h"
#include "constants.h"
#include "carSelectorState.h"
//http://www.cplusplus.com/doc/tutorial/templates/          <--------Visita esta página para entender la linea justo debajo de esta
template<> MenuState* Ogre::Singleton<MenuState>::msSingleton = 0;

using namespace std;
using namespace Ogre;

void MenuState::enter ()
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

    _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 1.0));
    createScene();
    _exitGame = false;
    //sounds::getInstance()->play_music("mainTheme");
}

void MenuState::exit ()
{
  _sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();
}

void MenuState::pause()
{
}

void MenuState::resume()
{
   enter();
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 1.0));
}

bool MenuState::frameStarted(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;
  return true;
}

bool MenuState::frameEnded(const Ogre::FrameEvent& evt)
{
   if (_exitGame) return false;
   else return true;
}

bool MenuState::keyPressed(const OIS::KeyEvent &e)
{
    // CUANDO TODO EL FLUJO DE ESTADOS FUNCIONE BIEN, HAY QUE HACER UN CLEAN UP DE TODO ESTO
  if (e.key == OIS::KC_SPACE || e.key==OIS::KC_P || e.key==OIS::KC_RETURN) // PLAY
  {
      sounds::getInstance()->play_effect("push");
      MyGUI::LayoutManager::getInstance().unloadLayout(layout);
      pushState(carSelectorState::getSingletonPtr()); 
  }
  else if (e.key == OIS::KC_U) { // USER CONTROLS
    sounds::getInstance()->play_effect("push");
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    pushState(ControlsState::getSingletonPtr());
  }
  else if (e.key == OIS::KC_G) { // GAME RANKINGS
    sounds::getInstance()->play_effect("push");
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    pushState(RecordsState::getSingletonPtr());
  }
  else if (e.key == OIS::KC_C) { // CREDITS
    sounds::getInstance()->play_effect("push");
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    pushState(CreditsState::getSingletonPtr());
  }

  else if (e.key == OIS::KC_ESCAPE || e.key==OIS::KC_E)  // EXIT
  {
    sounds::getInstance()->play_effect("push");
    _exitGame = true;
  }

//  else if (e.key == OIS::KC_S) {
//    sounds::getInstance()->play_effect("push");
//    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
//    pushState(OptionsState::getSingletonPtr());
//  }

  else if (e.key == OIS::KC_O) {                                // DEMO PAUSA
    sounds::getInstance()->play_effect("push");
    //MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    pushState(PauseState::getSingletonPtr());
  }
  else if (e.key == OIS::KC_W) {				// DEMO WIN
    sounds::getInstance()->play_effect("push");
    //MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    WinState::getSingletonPtr()->setPoints(123123);
    pushState(WinState::getSingletonPtr());
  }
  else if (e.key == OIS::KC_L) {				// DEMO LOOSE
    sounds::getInstance()->play_effect("push");
    //MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    LooseState::getSingletonPtr()->setPoints(111222);
    pushState(LooseState::getSingletonPtr());
  }
  else if (e.key == OIS::KC_R) {				// DEMO CARRUSEL
    sounds::getInstance()->play_effect("push");
    //MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    Carrusel c;
    c.go();
  }
  else if (e.key == OIS::KC_A) {				// DEMO PLAY WIDGETS
    sounds::getInstance()->play_effect("push");
    //PlayWidget play(NULL);
    PlayWidget * play = new PlayWidget();

  }
  return true;
}

bool MenuState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool MenuState::mouseMoved(const OIS::MouseEvent &e)
{
  return true;
}

bool MenuState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;

  if (btn_credits->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_C,0);
    MenuState::keyPressed(e);
  }
  else if (btn_records->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_G,0);
    MenuState::keyPressed(e);
  }
  else if (btn_play->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_P,0);
    MenuState::keyPressed(e);
  }
  else if (btn_controls->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_U,0);
    MenuState::keyPressed(e);
  }
  else if (btn_exit->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_E,0);
    MenuState::keyPressed(e);
  }

  return true;
}

bool MenuState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

MenuState* MenuState::getSingletonPtr()
{
    return msSingleton;
}

MenuState& MenuState::getSingleton()
{ 
  assert(msSingleton);
  return *msSingleton;
}

MenuState::~MenuState()
{
}

void MenuState::destroyMyGui()
{
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

string MenuState::get_high_score()
{
  string recordname;
  int recordpoints;
  char msg [128];
  records::getInstance()->getBest (recordname,recordpoints);
  sprintf (msg,"%s %d",recordname.c_str(),recordpoints);
  return string(msg);
} 

void MenuState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout(LAYOUT_MAIN);
  btn_records = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_records");
  btn_credits = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_credits");
  btn_play = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_play");
  btn_controls = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_controls");
  btn_exit = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_exit");
  edt_high = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("edt_high");
  //edt_high->setCaption(get_high_score()); 
}

void MenuState::createScene()
{
 createMyGui();
}

MenuState::MenuState()
{
}

bool MenuState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool MenuState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool MenuState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}
