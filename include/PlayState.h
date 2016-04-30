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

#ifndef PlayState_H
#define PlayState_H


#include <Ogre.h>
#include <OIS/OIS.h>
#include <string>
#include "GameState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsWorld.h"
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include <vector>
#include "OgreUtil.h"
#include "Wiimote.h"
#include "constants.h"

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;
class PlayState : public Ogre::Singleton<PlayState>, public GameState
{
 public:
  PlayState () {}
  ~PlayState();

  void enter ();
  void exit();
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
  static PlayState& getSingleton ();
  static PlayState* getSingletonPtr ();
  void set_lives (int lives);
  int  get_lives ();

  void set_score (int score);
  int  get_score ();

  void  game_over ();
  void  win ();
  void handleCollision(btCollisionObject *body0, btCollisionObject *body1);
  OgreBulletDynamics::RigidBody* getCameraBody(){return _cameraBody;};
  

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::SceneNode* _cameraNode;
  MyGUI::VectorWidgetPtr layout;
  void checkCollisions();
  

  bool _exitGame;
  int lives;
  int score;
  bool paused;
  Ogre::Real _deltaT;
  std::map<OIS::KeyCode,bool> keysArePressed;
  shared_ptr<DynamicsWorld> _world;
  DebugDrawer* _debugDrawer;
  RigidBody* _cameraBody;
  CollisionShape* _cameraShape;
  CollisionShape* _floorShape;
  RigidBody* _floorBody;

  

private:

  void createMyGui();
  void destroyMyGui();
  void createScene();
  void createLight();
  void moveCamera();

};

#endif
