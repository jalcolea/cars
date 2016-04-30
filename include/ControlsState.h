/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 *
 * Código modificado a partir de Managing Game States with OGRE
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 * Inspirado en Managing Game States in C++
 * http://gamedevgeek.com/tutorials/managing-game-states-in-c/
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef ControlsState_H
#define ControlsState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <string>
#include "GameState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"


using namespace std;
using namespace Ogre;
class ControlsState : public Ogre::Singleton<ControlsState>, public GameState
{
 public:
  ControlsState () {}
  ~ControlsState();

  void enter ();
  void exit ();
  void pause ();
  void resume ();

  bool keyPressed (const OIS::KeyEvent &e);
  bool keyReleased (const OIS::KeyEvent &e);

  bool mouseMoved (const OIS::MouseEvent &e);
  bool mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  bool mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);
  
/* WIIMOTE *********************************************************************/  
    bool WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e);
    bool WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e);
    bool WiimoteIRMove(const wiimWrapper::WiimoteEvent &e);
/*******************************************************************************/  

  // Heredados de Ogre::Singleton.
  static ControlsState& getSingleton ();
  static ControlsState* getSingletonPtr ();


 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;

  bool _exitGame;

  MyGUI::VectorWidgetPtr layout;
  MyGUI::Button* btn_back;
  MyGUI::Button* btn_mouse;
  MyGUI::Button* btn_wiimote;
  MyGUI::EditBox* text;  
  MyGUI::EditBox* edt_high;

private:
  private:
  void mostrarFondo();
  Ogre::TextureUnitState* CreateTextureFromImgWithoutStretch(const String& texName, Real texSize, const String& imgName);
  void createScene();
  void createMyGui();
  void destroyMyGui();
  void notifyButtonPress(MyGUI::Widget* _widget);
  string get_high_score();
};

#endif
