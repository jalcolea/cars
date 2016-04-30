
#ifndef Carrusel_H
#define Carrusel_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <string>
#include "GameState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"


using namespace std;
using namespace Ogre;
#define MAX_IMG 5

class Carrusel 
{
 public:
  Carrusel ();
  ~Carrusel();
  void go();
  MyGUI::VectorWidgetPtr count_layout;
  MyGUI::ImageBox* img[MAX_IMG];
  char * img_names[MAX_IMG]={"img_start","img_tres","img_dos","img_uno","img_go"};
  private:
  static void * run(void * arg);
};

#endif
