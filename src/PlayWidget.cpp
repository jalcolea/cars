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
#include "PlayWidget.h"

using namespace std;
using namespace Ogre;
using namespace std;

PlayWidget::PlayWidget()
{
  cout << __FUNCTION__ << endl;
  layout = MyGUI::LayoutManager::getInstance().loadLayout(LAYOUT_RACE);
  race_position = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("race_position");
  race_speed = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("race_speed");
  race_lap = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("race_lap");
  race_time = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("race_time");
  race_circuit= MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("race_circuit");
  start_thread=false;
}

PlayWidget::~PlayWidget()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void PlayWidget::lap(int lap, int max_lap)
{
  char buff [128];
  sprintf(buff,"%d:%d",lap,max_lap);
  race_lap->setCaption(buff);
}

void PlayWidget::speed(int speed)
{
  char buff [128];
  sprintf(buff,"%d",speed);
  race_speed->setCaption(buff);
}

void PlayWidget::position(int pos, int max_pos)
{
  char buff [128];
  sprintf(buff,"%d:%d",pos,max_pos);
  race_position->setCaption(buff);
}

void PlayWidget::circuit(string circuit)
{
  race_circuit->setCaption(circuit);
}

void * PlayWidget::timer(void * data)
{
  PlayWidget * obj  = (PlayWidget *)data;
  int time=0;
  while (1)
  {
    obj->timer(time++);
    sleep(1);
  }
  return NULL;
}

void PlayWidget::startTime ()
{
  pthread_t thread;
  start_thread=true;
  int ret=pthread_create(&thread,NULL,PlayWidget::timer,this); 
}

void PlayWidget::stopTime ()
{
  start_thread=false;
}

void PlayWidget::timer (int time)
{
  char buff [128];
  int seconds;
  int minutes;

  seconds = time%60;
  minutes = time/60;

  sprintf(buff,"%02d:%02d",minutes,seconds);
  race_time->setCaption(buff);
}
