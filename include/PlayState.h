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
#include "CarRayCast.h"
#include "track.h"
#include "IAPointsDeserializer.h"
#include "pathDrawerState.h"
#include "cpuPlayer.h"
#include "humanPlayer.h"
#include "PlayWidget.h"
#include "soundUtil.h"


using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

#define CPU_PLAYERS 3
#define LAPS 3
#define Z_CAMARA_SEMICENITAL 30
#define MAX_TIME_CARRERA_ACABADA 2
#define MAX_CANALES_POR_COCHE 3


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
        
        std::vector<Vector3> posSalida { Vector3(-4,2.1,-22), Vector3(-4,2.1,-24), Vector3(-1,2.1,-22), Vector3(-1,2.1,-24) };
        
        void set_lives (int lives);
        int  get_lives ();

        void set_score (int score);
        int  get_score ();

        void  game_over ();
        void  win ();
        
        PlayWidget* _play;
        Ogre::Real _tiempoCarreraJugador = 0;

    protected:
        Ogre::Root *_root;
        Ogre::SceneManager *_sceneMgr;
        Ogre::Viewport *_viewport;
        Ogre::Camera *_camera;
        unique_ptr<track> _track;
        shared_ptr<OgreBulletDynamics::DynamicsWorld> _world;
        std::vector< unique_ptr<cpuPlayer> > _vCarsCpuPlayer;
        unique_ptr<humanPlayer> _humanPlayer;
        CollisionShape* _floorShape;
        RigidBody* _floorBody;
        bool _freeCamera = false;
        bool _playSimulation = true;
        bool _travellingCamara = true;
        bool _inicioCarrera = true;
        SceneNode* _nodoVista;
        SceneNode* _planeRoadNode;
        OgreBulletDynamics::RigidBody* _planeRoadBody;


        bool _exitGame;
        Ogre::Real _deltaT;
        MyGUI::VectorWidgetPtr layout;
        camara_view _vista = camara_view::SEMICENITAL;
     

        int lives;
        int score;
        bool paused;

    private:
        void createLight();
        void createMyGui();
        void destroyMyGui();
        void createScene();
        void createFloor();
        void createAtrezzo();
        void cargarParametros(string archivo, bool consoleOut);
        void configurarCamaraPrincipal();
        void colocaCamara(); // Para cambiar los tipos de vista de la cámara.
        void reposicionaCamara(); // moverá la cámara en función del tipo de vista actual.
        void createPlaneRoad();
        void initBulletWorld(bool showDebug);    
        void gestionaAnimaciones(Ogre::AnimationState *&anim, Ogre::Real deltaT, const String &nombreEnt, const String &nombreAnim);
        void createPlayersCPU();
        void createHumanPlayer();
        void createVallaVirtual();
        //void createOverlay();
        void pintaOverlayInfo();
        void flagKeys(bool flag);
        void updateCPU();
        void travellingCamara();
        void controlDeCarrera();
        void reparteCanalesSonido();
        void actualizaDistanciasParaSonido();
        void reproduceSonidosColisiones();
        double getCollisionForce(btPersistentManifold* maniFold);
        void activarCarrusel();
        Ogre::OverlayManager* _overlayManager;
        Ogre::Vector3 _vt;
        Ogre::Real _r;
        size_t _velocidad;
        size_t _fps;
        size_t _keys = static_cast<size_t>(keyPressed_flags::NONE);
        DebugDrawer* _debugDrawer;
        size_t _cursorVehiculo; // veremos si hace falta
        string _nombreTipoCoche; // para cargar la info del xml este nombre debe coincidir con la definición de uno definido en dicho xml
        string _nombreMaterial; // nombre del material que ha elegido el usuario en el selector de coches.
        bool _finalCarrera = false;
        size_t _posicionAlFinalCarrera = 0;
        Ogre::Real _timeCarreraAcabada = 0;
        int _canalActualReparto;
        
        void dibujaLinea(size_t idFrom, size_t idTo); // SOLO PARA COMPROBAR FUNCIONAMIENTO IA, QUITAR LUEGO
        std::vector<marquita> vMarcas;
        std::vector<size_t> _vranking;
        std::vector<canalesSonidoCoche> _vCanalesSonido;
        std::vector<btCollisionObject*> _bodies;

};

#endif
