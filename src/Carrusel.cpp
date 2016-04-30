#include "MenuState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "PlayState.h"
#include "ControlsState.h"
#include "RecordsState.h"
#include "CreditsState.h"
#include "PauseState.h"
#include "WinState.h"
#include "LooseState.h"
#include "records.h"
#include "Carrusel.h"

using namespace std;
using namespace Ogre;
using namespace std;

Carrusel::Carrusel()
{
cout << __FUNCTION__ << endl;
count_layout = MyGUI::LayoutManager::getInstance().loadLayout("shooter_countdown.layout");
}

Carrusel::~Carrusel()
{
cout << __FUNCTION__ << endl;
//  MyGUI::LayoutManager::getInstance().unloadLayout(count_layout);
}

void * Carrusel::run( void* arg)
{
MyGUI::ImageBox* start;
MyGUI::ImageBox* three;
MyGUI::ImageBox* two;
MyGUI::ImageBox* one;
MyGUI::ImageBox* go;
start = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_start");
three = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_tres");
two = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_dos");
one = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_uno");
go = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_go");
start->setVisible(true);
sounds::getInstance()->play_effect("push");
sleep(1);
start->setVisible(false); three->setVisible(true);
sounds::getInstance()->play_effect("push");
sleep(1);
three->setVisible(false); two->setVisible(true);
sounds::getInstance()->play_effect("push");
sleep(1);
two->setVisible(false); one->setVisible(true);
sounds::getInstance()->play_effect("push");
sleep(1);
one->setVisible(false);go->setVisible(true);
sounds::getInstance()->play_effect("push");
sleep(1);
go->setVisible(false);
sounds::getInstance()->play_effect("push");
  return NULL;
}

void Carrusel::go()
{
  pthread_t thread1, thread2;
  int  iret1;
  iret1 = pthread_create( &thread1, NULL, Carrusel::run, this);
}
