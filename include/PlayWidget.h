
#ifndef PLAYWIDGET_H
#define PLAYWIDGET_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <string>
#include <pthread.h>

#include "GameState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"


using namespace std;
using namespace Ogre;
#define MAX_IMG 5

class PlayWidget 
{
 public:
  PlayWidget ();
  ~PlayWidget();
  void lap(int lap, int max_lap);
  void speed(int speed);
  void position(int pos, int max_pos);
  void circuit(string circuit);
  void timer(int time);
  void startTime ();
  void stopTime ();
  static void * timer ( void * data);
  bool start_thread;
  inline int getTime(){ return _time; };
  inline void pauseTimer(){ _pause = true; };
  inline void resumeTimer(){ _pause = false; };
  
  static pthread_t thread;
  
  protected:
   
  private:
  MyGUI::VectorWidgetPtr layout;
  MyGUI::TextBox * race_position;
  MyGUI::TextBox * race_speed;
  MyGUI::TextBox * race_lap;
  MyGUI::TextBox * race_time;
  MyGUI::TextBox * race_circuit;
  int _time;
  bool _pause;
  
  
};

#endif
