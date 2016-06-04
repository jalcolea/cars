#ifndef CARSELECTORSTATE_H
#define CARSELECTORSTATE_H

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


#define MAX_ALTURA_SELECCIONADO 0.5
#define MIN_ALTURA_SELECCIONADO 0
#define SPEED_MOVIMIENTOS 10.0f
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

class  carSelectorState : public Ogre::Singleton< carSelectorState>, public GameState
{
public:
     carSelectorState(){}
    ~ carSelectorState();
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
    static carSelectorState& getSingleton();
    static carSelectorState* getSingletonPtr();

    string getNombreTipoCocheSeleccionado();
    inline string getNombreMaterialSeleccionado(){ return _vMateriales[_idMaterialActual]; };
    inline std::vector<string>& getMateriales() { return _vMateriales; }

protected:
    Ogre::Root *_root;
    Ogre::SceneManager *_sceneMgr;
    Ogre::Viewport *_viewport;
    Ogre::Camera *_camera;
    SceneNode* _track;
    std::vector< SceneNode* > _vCars;
    SceneNode* _nodoSelector;

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
    void girarRuleta();
    void inicializarEstadoRotacionSelector(Real factorRot, Quaternion orientacionOriginal, Real angulo, Vector3 eje, int sentido);
    void bajarDeseleccionado();
    void subirSeleccionado();
    void cambiarMaterialVehicSeleccionado();
    std::vector<string> _vMateriales {"carmaterial_blue","carmaterial_citrus","carmaterial_green","carmaterial_orange","carmaterial_silver","carmaterial_violet","red.gloss"};
    std::vector<Entity*> _vEntCars;
    int _cursorVehiculo; 
    int _idVehicSubiendo;
    int _idVehicBajando;
    bool _girandoRuleta;
    bool _bajando;
    bool _subiendo;
    int _sentidoGiro;
    int _idMaterialActual;
    Quaternion _orientOriginal;
    Quaternion _orientDestino;
    Real _progresoRotacion;
    Real _factorRotacion;
    Real _progresoBajada;
    Real _progresoSubida;

};

#endif // CARSELECTORSTATE_H
