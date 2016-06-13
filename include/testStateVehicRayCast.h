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

#ifndef testStateVehicRayCast_H
#define testStateVehicRayCast_H

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
#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "OgreBulletDynamicsWorld.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "SceneNodeConfig.h"
#include "car.h"
#include "track.h"
#include "CarRayCast.h"
#include "OgreUtil.h"

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


//enum class keyPressed_flags
//{
//    NONE =           0,
//    LEFT =      1 << 0,
//    RIGHT =     1 << 1,
//    UP =        1 << 2,
//    DOWN =      1 << 3,
//    INS =       1 << 4,
//    DEL =       1 << 5,
//    PGUP =      1 << 6,
//    PGDOWN =    1 << 7,
//    NUMPAD1 =   1 << 8,
//    NUMPAD2 =   1 << 9,
//    NUMPAD3 =   1 << 10,
//    NUMPAD5 =   1 << 11
//};
//
//enum class camara_view : int
//{
//    SEMICENITAL,
//    TRASERA_ALTA,
//    TRASERA_BAJA,
//    INTERIOR,
//    TOTAL_COUNT       // Su valor será el número total de elementos de esta enum. Útil para modular.
//};

class testStateVehicRayCast : public Ogre::Singleton<testStateVehicRayCast>, public GameState
{
public:
    testStateVehicRayCast(){}
    ~testStateVehicRayCast();
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
    static testStateVehicRayCast &getSingleton();
    static testStateVehicRayCast *getSingletonPtr();
    
/* PARA MANEJAR COLISIONES (NO FUNCIONAN BIEN)**********************************/          
//    void handleCollision(btCollisionObject *body0, btCollisionObject *body1);
/*******************************************************************************/      

    bool compruebaCheckPoint();

protected:
    Ogre::Root *_root;
    Ogre::SceneManager *_sceneMgr;
    Ogre::Viewport *_viewport;
    Ogre::Camera *_camera;
    //SceneNodeConfig _scn;
    unique_ptr<car> _car;
    unique_ptr<track> _track;
    unique_ptr<CarRayCast> _carRayCast;
    shared_ptr<OgreBulletDynamics::DynamicsWorld> _world;
    std::vector< unique_ptr<CarRayCast> > _vCarsRayCast;
    CollisionShape* _floorShape;
    RigidBody* _floorBody;
    RigidBody* _bodyCheckPoint;
    bool _freeCamera = false;
    bool _playSimulation = true;
    SceneNode* _nodoVista;


    bool _exitGame;
    Ogre::Real _deltaT;
    MyGUI::VectorWidgetPtr layout;
    camara_view _vista = camara_view::SEMICENITAL;

private:

    void createLight();
    void createMyGui();
    void destroyMyGui();
    void createScene();
    void createFloor();
    void cargarParametros(string archivo, bool consoleOut);
    void configurarCamaraPrincipal();
    void colocaCamara(); // Para cambiar los tipos de vista de la cámara.
    void reposicionaCamara(); // moverá la cámara en función del tipo de vista actual.
    void createPlaneRoad();
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
    size_t _cursorVehiculo; // para el patio de pruebas, indica que objeto coche del vector de coches se maneja.

};

#endif
