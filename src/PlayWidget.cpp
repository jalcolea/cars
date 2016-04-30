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

#define DEFAULT_LAYOUT "shooter_play.layout"
PlayWidget::PlayWidget(char * layout_id)
{
cout << __FUNCTION__ << endl;
layout = MyGUI::LayoutManager::getInstance().loadLayout((layout_id==NULL)?DEFAULT_LAYOUT:layout_id);
edit_points = MyGUI::Gui::getInstance().findWidget<MyGUI::TextBox>("edit_points");
progress_live = MyGUI::Gui::getInstance().findWidget<MyGUI::ProgressBar>("progress_live");
edit_points->setCaption("123123");
progress_live->setProgressPosition(10);
}

PlayWidget::~PlayWidget()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

void PlayWidget::setPoints(int points)
{
  char buff [128];

  sprintf(buff,"%d",points);
  edit_points->setCaption(buff);
}

void PlayWidget::setLive(int level)
{
  if (level>100) level=100;
  progress_live->setProgressPosition(level);
}

