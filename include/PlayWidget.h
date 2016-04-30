
#ifndef PLAYWIDGET_H
#define PLAYWIDGET_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <string>
#include "GameState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"


using namespace std;
using namespace Ogre;
#define MAX_IMG 5

class PlayWidget 
{
 public:
  PlayWidget ( char * layout_id);
  ~PlayWidget();
  void setPoints(int points);
  void setLive(int level);

  protected:
   
  private:
  MyGUI::VectorWidgetPtr layout;
  MyGUI::ProgressBar * progress_live;
  MyGUI::TextBox * edit_points;
};

#endif
