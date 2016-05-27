    /*********************************************************************n
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

#ifndef testState_H
#define testState_H

#include <Ogre.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OIS/OIS.h>
#include "GameState.h"
#include "sounds.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "OgreBulletCollisionsShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBulletDynamicsWorld.h"
#include "SceneNodeConfig.h"
#include "car.h"
#include "track.h"
#include "CarRayCast.h"

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


enum class keyPressed_flags
{
    NONE = 0,
    LEFT = 2,
    RIGHT = 4,
    UP = 8,
    DOWN = 16,
    INS = 32,
    DEL = 64,
    PGUP = 128,
    PGDOWN = 256
};

class testState : public Ogre::Singleton<testState>, public GameState
{
public:
    testState(){}
    ~testState();
    void enter();
    void exit();
    void pause();
    void resume();
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool frameStarted(const Ogre::FrameEvent &evt);
    bool frameEnded(const Ogre::FrameEvent &evt);
    
/* WIIMOTE *********************************************************************/  
    bool WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e);
    bool WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e);
    bool WiimoteIRMove(const wiimWrapper::WiimoteEvent &e);
/*******************************************************************************/  

    // Heredados de Ogre::Singleton.
    static testState &getSingleton();
    static testState *getSingletonPtr();

protected:
    Ogre::Root *_root;
    Ogre::SceneManager *_sceneMgr;
    Ogre::Viewport *_viewport;
    Ogre::Camera *_camera;
    SceneNodeConfig _scn;
    unique_ptr<car> _car;
    unique_ptr<track> _track;
    unique_ptr<CarRayCast> _carRayCast;
    shared_ptr<OgreBulletDynamics::DynamicsWorld> _world;
    CollisionShape* _floorShape;
    RigidBody* _floorBody;
    bool _freeCamera = false;

    bool _exitGame;
    Ogre::Real _deltaT;
    MyGUI::VectorWidgetPtr layout;

private:

    void createLight();
    void createMyGui();
    void destroyMyGui();
    void createScene();
    void createFloor();
    void cargarParametros(string archivo, bool consoleOut);
    void configurarCamaraPrincipal();
    void initBulletWorld(bool showDebug);    
    void gestionaAnimaciones(Ogre::AnimationState *&anim, Ogre::Real deltaT, const String &nombreEnt, const String &nombreAnim);
    TextureUnitState *CreateTextureFromImgWithoutStretch(const String &texName, Real texSize, const String &imgName);
    void createOverlay();
    void pintaOverlayInfo();
    void flagKeys(bool flag);
    Ogre::OverlayManager* _overlayManager;
    Ogre::Vector3 _vt;
    Ogre::Real _r;
    size_t _velocidad;
    size_t _fps;
    size_t _keys = static_cast<size_t>(keyPressed_flags::NONE);
    DebugDrawer* _debugDrawer;

};

#endif
