#ifndef PATHDRAWERSTATE_H
#define PATHDRAWERSTATE_H

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


#define MASK_ESCENARIO 1 << 0
#define MASK_CIRCUITO  1 << 1
#define MASK_MARCA     1 << 2

struct marquita
{
    SceneNode* _nodoMarca;
    Entity* _entMarca;
    string _nombreNodo;
    string _nombreEnt;
    size_t _id;
    Quaternion rotacion;
};

class pathDrawerState : public Ogre::Singleton<pathDrawerState>, public GameState
{
public:
     pathDrawerState(){}
    ~ pathDrawerState();
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
    static pathDrawerState& getSingleton();
    static pathDrawerState* getSingletonPtr();


protected:
    Ogre::Root *_root;
    Ogre::SceneManager *_sceneMgr;
    Ogre::Viewport *_viewport;
    Ogre::Camera *_camera;
    SceneNode* _track;
    SceneNode* _nodoSelector;
    SceneNode* _planeRoadNode;
    nodoOgre_t _checkPointInfo;
    
    std::vector<marquita> vMarcas;

    bool _exitGame;
    Ogre::Real _deltaT;
    MyGUI::VectorWidgetPtr layout;
    
    string extraeNombreSinID(string& nombre);

private:

    void createLight();
    void createMyGui();
    void destroyMyGui();
    void createScene();
    void createFloor();
    void createPlaneRoad();
    void cargarParametros(string archivo, bool consoleOut);
    void configurarCamaraPrincipal();
    void addMarca(Vector3 posicion); // en fase de deprecado
    void addCheckPoint(Vector3 posicion, Quaternion q);
    void dibujaLinea(size_t idFrom, size_t idTo);
    void recolocarLinea();
    void guardarRuta();
    void cargarRuta(string fichero);  // en fase de deprecado
    void borrarTodasLasMarcas();
    void cargarRutaCheckPoint(string fichero);
    RaySceneQuery* _raySceneQuery;
    size_t _idMarca;
    bool _crearMarca;
    bool _rotarMarca;
    
    Ray setRayQuery(int posx, int posy, uint32 mask);

};

#endif // PATHDRAWERSTATE_H
