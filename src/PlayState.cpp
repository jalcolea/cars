#include "RecordsState.h"
#include "PauseState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "OgreBulletCollisionsShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBulletDynamicsWorld.h"
#include <string>
#include <vector>
#include "records.h"
#include "OgreUtil.h"
#include <ctime>
#include "OgreOverlayManager.h"
#include "OgreOverlaySystem.h"
#include "carSelectorState.h"

#define CAMSPEED 20
#define CAMROTATESPEED 0.1
template <> PlayState *Ogre::Singleton<PlayState>::msSingleton = 0;

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

void PlayState::enter() 
{
   // Recuperar recursos básicos
    _root = Ogre::Root::getSingletonPtr();
    try
    {
        _sceneMgr = _root->getSceneManager("SceneManager");
    }
    catch (...)
    {
        cout << "SceneManager no existe, creándolo \n";
        _sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
    }

    try
    {
        _camera = _sceneMgr->getCamera("IntroCamera");
    }
    catch (...)
    {
        cout << "testCamera no existe, creándola \n";
        _camera = _sceneMgr->createCamera("IntroCamera");
    }

    try
    {
        _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
    }
    catch (...)
    {
        _viewport = _root->getAutoCreatedWindow()->getViewport(0);
    }    
    
    _exitGame = false;
    paused = false;
    _deltaT = 0;

    cout << "tipo coche seleccionado: " << carSelectorState::getSingletonPtr()->getNombreTipoCocheSeleccionado() << endl;
    cout << "material seleccionado: " << carSelectorState::getSingletonPtr()->getNombreMaterialSeleccionado()<< endl;
    
    _sceneMgr->clearScene();

  
    //Color de fondo inicial
    //_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));

    // Cargar los parámetros para construir elementos del juego (coches, circuitos, camaras, etc)
    cargarParametros("SceneNodes.xml",true);
    
    // Configurar camara
    configurarCamaraPrincipal();

    // Activar Bullet
    initBulletWorld(false);

    //Preparar escena
    createScene();
}

void PlayState::exit() { destroyMyGui(); }

void PlayState::pause() { paused = true; }

void PlayState::resume()
{ 
    paused = false; 
}

bool PlayState::frameStarted(const Ogre::FrameEvent &evt) {

    _deltaT = evt.timeSinceLastFrame;
    if (_playSimulation) _world.get()->stepSimulation(_deltaT);
    static Ogre::Real speed = 10.0;
    _fps = 1.0 / _deltaT;
    _r = 0;
    Real rr = 0;
    Real rSteer = 0;
    Real sCar = 0;
    static Real sBrake = 10;
    _vt = Ogre::Vector3::ZERO;

    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_P)) _playSimulation = !_playSimulation;

    if (_keys & static_cast<size_t>(keyPressed_flags::LEFT))  _vt.x += -1;
    if (_keys & static_cast<size_t>(keyPressed_flags::RIGHT)) _vt.x += 1;
    if (_keys & static_cast<size_t>(keyPressed_flags::UP))    _vt.y += 1;
    if (_keys & static_cast<size_t>(keyPressed_flags::DOWN))  _vt.y += -1;
    if (_keys & static_cast<size_t>(keyPressed_flags::INS))   _vt.z += 1;
    if (_keys & static_cast<size_t>(keyPressed_flags::DEL))   _vt.z += -1;
    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_HOME)) speed =0.5;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_END)) speed =10;

    _camera->moveRelative(_vt * _deltaT * speed);//10.0 /*tSpeed*/);-l
    if (_camera->getPosition().length() < 2.0) 
        _camera->moveRelative(-_vt * _deltaT * speed);//10.0 /*tSpeed*/);
        
    if (_keys & static_cast<size_t>(keyPressed_flags::PGUP)) _r += 180;
    if (_keys & static_cast<size_t>(keyPressed_flags::PGDOWN)) _r += -180;
        
    _camera->pitch(Ogre::Radian(_r * _deltaT * 0.005));
    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_8)) rr+=180;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_9)) rr-=180;
    _camera->yaw(Ogre::Radian(rr * _deltaT * 0.005));

    if (!_freeCamera)
        reposicionaCamara();


  return !_exitGame;
}

bool PlayState::frameEnded(const Ogre::FrameEvent &evt) { return true; }

bool PlayState::keyPressed(const OIS::KeyEvent &e) 
{
      if (e.key == OIS::KC_L)
    {    _freeCamera = !_freeCamera; cout << "Camara libre: " << _freeCamera << endl; }
    
    if (e.key == OIS::KC_C)
    {
        // Las enum class serán recomendables pero según que código queda horrible, vaya tela.
        _vista = static_cast<camara_view>(static_cast<int>(_vista)+1);
        _vista = static_cast<camara_view>(static_cast<int>(_vista) % static_cast<int>(camara_view::TOTAL_COUNT));
        colocaCamara();
    }    
    
    if (e.key == OIS::KC_V)
    {
        _cursorVehiculo += 1;
        _cursorVehiculo %= _vCarsRayCast.size();
    }    
    
    if (e.key == OIS::KC_R)
        _vCarsRayCast[_cursorVehiculo]->recolocar(_vCarsRayCast[_cursorVehiculo]->getPosicionActual());

    flagKeys(true);
       
    return true;
}

bool PlayState::keyReleased(const OIS::KeyEvent &e) 
{ 
    flagKeys(false);

    if (e.key == OIS::KC_ESCAPE)
        _exitGame = true;
    
    return true; 
}

void PlayState::createLight()
{
    //_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
    _sceneMgr->setShadowTextureCount(2);
    _sceneMgr->setShadowTextureSize(512);
    Light *light = _sceneMgr->createLight("Light1");
    light->setPosition(30, 30, 0);
    light->setType(Light::LT_SPOTLIGHT);
    light->setDirection(Vector3(-1, -1, 0));
    light->setSpotlightInnerAngle(Degree(60.0f));
    light->setSpotlightOuterAngle(Degree(80.0f));
    light->setSpotlightFalloff(0.0f);
    light->setCastShadows(true);
}

void PlayState::win() {}

void PlayState::game_over() {}

void PlayState::set_lives(int lives) { this->lives = lives; }

int PlayState::get_lives() { return lives; }

void PlayState::set_score(int score) { this->score = score; }

int PlayState::get_score() { return score; }

bool PlayState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e){ return true; }
bool PlayState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e){ return true; }
bool PlayState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e){ return true; }

void PlayState::cargarParametros(string archivo, bool consoleOut)
{   
    //Cargamos la info para generar elementos de la escena (scenenodes, cameras, lights)
    SceneNodeConfig::getSingleton().load_xml("SceneNodes.xml");
    
    map_nodos_t nodos = SceneNodeConfig::getSingleton().getMapNodos();
    map_cameras_t camaras = SceneNodeConfig::getSingleton().getMapCameras();
    map_vehiculos_ray_cast_t vehicRayCast = SceneNodeConfig::getSingleton().getMapVehiculosRaycast();
    
    if (consoleOut)
    {
        for (it_map_nodos it = nodos.begin(); it != nodos.end(); ++it)
                // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
                // first sería la clave y second el valor.
                cout << (*it).second << endl;
                
        for (it_map_cameras it = camaras.begin(); it != camaras.end(); ++it)
                // cada elemento de it_map_nodos es un tipo pair<tipo1 first,tipo2 second> donde
                // first sería la clave y second el valor.
                cout << (*it).second << endl;
                
        for (it_map_vehiculos_ray_cast it = vehicRayCast.begin(); it != vehicRayCast.end(); ++it)
            cout << (*it).second << endl;
    }
}

void PlayState::configurarCamaraPrincipal()
{
    //Configuramos la camara
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    nodoCamera_t cam = SceneNodeConfig::getSingleton().getInfoCamera("IntroCamera");
    _camera->setAspectRatio(width / height);
    _camera->setPosition(cam.posInicial);
    _camera->lookAt(cam.lookAt);
    _camera->setNearClipDistance(cam.nearClipDistance);
    _camera->setFarClipDistance(cam.farClipDistance);
    _nodoVista = _sceneMgr->createSceneNode("nodoVista");
    cout << "padre nodovista: " << _nodoVista->getParent() << endl;
    cout << "padre _camara: " << _camera->getParentNode() << endl;
}

void PlayState::initBulletWorld(bool showDebug)
{
    _debugDrawer = new OgreBulletCollisions::DebugDrawer();
    _debugDrawer->setDrawWireframe(true);
    SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("debugNode", Vector3::ZERO);
    node->attachObject(static_cast<SimpleRenderable *>(_debugDrawer));
    
//    AxisAlignedBox boundBox = AxisAlignedBox(Ogre::Vector3(-10000, -10000, -10000),Ogre::Vector3(10000, 10000, 10000));
    AxisAlignedBox boundBox = AxisAlignedBox(Ogre::Vector3(-100, -100, -100),Ogre::Vector3(100, 100, 100));
    _world = shared_ptr<OgreBulletDynamics::DynamicsWorld>(new DynamicsWorld(_sceneMgr, boundBox, Vector3(0, -9.8, 0))); //, true, true, 15000));
    _world.get()->setDebugDrawer(_debugDrawer);
    _world.get()->setShowDebugShapes(showDebug);
}

void PlayState::createScene()
{
  
    _sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    _sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
    _sceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
    _sceneMgr->setShadowFarDistance(100);
    _sceneMgr->setSkyBox(true, "skybox");

    //createOverlay();
    createLight();
    createFloor();
    //createMyGui();
    
    _track = unique_ptr<track>(new track("track1NoRoadBig",_world.get(),Vector3(0,0,0),_sceneMgr));
    createPlaneRoad();
    //_carRayCast = unique_ptr<CarRayCast>(new CarRayCast("parsche-sport",Vector3(0,0,0),_sceneMgr,_world.get()));
    //_carRayCast->buildVehiculo();
    
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("kart",Vector3(0,0,0),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("farara-sport",Vector3(0,0,0),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("formula",Vector3(0,0,0),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("groupC1",Vector3(0,0,0),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("groupC2",Vector3(0,0,0),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("lamba-sport",Vector3(0,0,0),_sceneMgr,_world.get())));
    _vCarsRayCast.push_back(unique_ptr<CarRayCast>(new CarRayCast("parsche-sport",Vector3(0,0,0),_sceneMgr,_world.get())));
    
    for (auto it = _vCarsRayCast.begin(); it != _vCarsRayCast.end(); ++it)
        (*it)->buildVehiculo();
    
    _cursorVehiculo = 0;
    
    // Carga de la malla que bordea el circuito para que no se salga el coche, SOLO PARA PRUEBAS
//    nodoOgre_t nodoConfigCol = _scn.getInfoNodoOgre("track1colLateral");
//    Entity* entColLateral = _sceneMgr->createEntity(nodoConfigCol.nombreEntidad, nodoConfigCol.nombreMalla);
//    SceneNode* nodoColLateral = _sceneMgr->createSceneNode(nodoConfigCol.nombreNodo);
//    nodoColLateral->attachObject(entColLateral);
//    _track->getSceneNode()->addChild(nodoColLateral);
//    OgreBulletCollisions::StaticMeshToShapeConverter* trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entColLateral);
//    OgreBulletCollisions::TriangleMeshCollisionShape* tri = trimeshConverter->createTrimesh();
//    OgreBulletDynamics::RigidBody* body = new OgreBulletDynamics::RigidBody(nodoConfigCol.nombreNodo, _world.get(), COL_TRACK,  COL_CAMERA | COL_FLOOR | COL_CAR | COL_TRACK_COLISION);
//    body->setShape(nodoColLateral,tri,nodoConfigCol.bodyRestitutionBullet,nodoConfigCol.frictionBullet,nodoConfigCol.masaBullet,nodoConfigCol.posShapeBullet);
//    nodoColLateral->setVisible(false);


//******************************************************************************************************************
//**SOLO PARA VER SI LA IA FUNCIONA QUITAR DESPUÉS DE QUE FUNCIONE**************************************************
 
    IAPointsDeserializer iapd;
    iapd.cargarFichero("rutasIA.xml");
    std::vector<iacomplexpoint> vpoints = iapd.getPoints();
    
    for (size_t i = 0; i < vpoints.size(); i++)
    {
      //Vector3 posicion(vpoints[i].base.x(),vpoints[i].base.y(),vpoints[i].base.z());
      Vector3 posicion(vpoints[i].base.x(),_planeRoadNode->getPosition().y,vpoints[i].base.z());
      marquita marca;
      marca._nombreNodo = "nodoMarca_" + to_string(i);
      marca._nodoMarca = _sceneMgr->createSceneNode(marca._nombreNodo);
      marca._nombreEnt = "entMarca_" + to_string(i);                  
      marca._entMarca = _sceneMgr->createEntity(marca._nombreEnt,"marca.mesh");
      marca._entMarca->setCastShadows(false);
      marca._entMarca->setQueryFlags(MASK_MARCA);
      marca._nodoMarca->attachObject(marca._entMarca);
      marca._nodoMarca->translate(posicion);
      _sceneMgr->getRootSceneNode()->addChild(marca._nodoMarca);
      marca._id = i;
      
      vMarcas.push_back(marca);
      
      if (marca._id > 0) dibujaLinea(marca._id -1, marca._id);
      
      cout << "nombre nodo marca creado: " << marca._nodoMarca->getName() << endl;
      cout << "nombre entity marca creado: " << marca._entMarca->getName() << endl;
      cout << "posicion de la marca creada: " << marca._nodoMarca->getPosition() << endl;
    }    

  
}

void PlayState::dibujaLinea(size_t idFrom, size_t idTo)
{
    ManualObject* manual = _sceneMgr->createManualObject("line_" + to_string(idFrom));
     
    manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
        manual->position(vMarcas[idFrom]._nodoMarca->getPosition()); //start
        manual->position(vMarcas[idTo]._nodoMarca->getPosition());    //end
    manual->end();
     
    _sceneMgr->getRootSceneNode()->attachObject(manual);
    cout << "nombre entity linea: " << manual->getName() << endl;
}


void PlayState::createFloor() 
{
    SceneNode *floorNode = _sceneMgr->createSceneNode("floor");
    Plane planeFloor;
    planeFloor.normal = Vector3(0, 1, 0);
    planeFloor.d = 2;
    MeshManager::getSingleton().createPlane("FloorPlane", 
                                          ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                          planeFloor, 200000, 200000, 20, 20, true, 1, 9000, 9000, Vector3::UNIT_Z);
    Entity *entFloor = _sceneMgr->createEntity("floor", "FloorPlane");
    entFloor->setCastShadows(true);
    entFloor->setMaterialName("floor");
    floorNode->attachObject(entFloor);
    _sceneMgr->getRootSceneNode()->addChild(floorNode);
    _floorShape = new StaticPlaneCollisionShape(Ogre::Vector3(0, 1, 0), 0);
    // Con VehicleRaycast no se pueden usar máscaras de colisión, no funcionan y los coches no colisionan con nada.
//    _floorBody = new RigidBody("rigidBodyPlane", _world.get(), COL_FLOOR, COL_CAMERA | COL_CAR | COL_TRACK | COL_TRACK_COLISION);
    _floorBody = new RigidBody("rigidBodyPlane", _world.get());

    _floorBody->setStaticShape(_floorShape, 0.5, 0.8);
    floorNode->setPosition(Vector3(0, 2, 0));
}

void PlayState::createPlaneRoad()
{
    nodoOgre_t nodoXML = SceneNodeConfig::getSingleton().getInfoNodoOgre("PlaneRoadBig");
    //SceneNode* planeRoadNode = _sceneMgr->createSceneNode(nodoXML.nombreNodo);
    _planeRoadNode = _sceneMgr->createSceneNode(nodoXML.nombreNodo);
    Entity* planeRoadEnt = _sceneMgr->createEntity(nodoXML.nombreEntidad,nodoXML.nombreMalla);
    planeRoadEnt->setCastShadows(true);
    _planeRoadNode->attachObject(planeRoadEnt);
    _sceneMgr->getRootSceneNode()->addChild(_planeRoadNode);
    
    OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(planeRoadEnt);
    OgreBulletCollisions::TriangleMeshCollisionShape *roadTrimesh = trimeshConverter->createTrimesh();
    OgreBulletDynamics::RigidBody *planeRoadBody = new OgreBulletDynamics::RigidBody(nodoXML.nombreNodo, _world.get());
    //planeRoadBody->setShape(planeRoadNode, roadTrimesh, 0.8, 0.95, 0, Vector3(0,0.001,0));
    planeRoadBody->setShape(_planeRoadNode, roadTrimesh,nodoXML.frictionBullet, nodoXML.bodyRestitutionBullet, nodoXML.masaBullet, nodoXML.posInicial);
     
}

void PlayState::flagKeys(bool flag)
{
    if (flag)
    {
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP))
            _keys |= static_cast<size_t>(keyPressed_flags::UP);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN))
            _keys |= static_cast<size_t>(keyPressed_flags::DOWN);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT))
            _keys |= static_cast<size_t>(keyPressed_flags::LEFT);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
            _keys |= static_cast<size_t>(keyPressed_flags::RIGHT);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_INSERT))
            _keys |= static_cast<size_t>(keyPressed_flags::INS);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DELETE))
            _keys |= static_cast<size_t>(keyPressed_flags::DEL);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGUP))
            _keys |= static_cast<size_t>(keyPressed_flags::PGUP);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGDOWN))
            _keys |= static_cast<size_t>(keyPressed_flags::PGDOWN);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD1))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD1);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD2))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD2);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD3))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD3);
        if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD5))
            _keys |= static_cast<size_t>(keyPressed_flags::NUMPAD5);
    }
    else
    {
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP))
            _keys = ~(~_keys | static_cast<size_t>(keyPressed_flags::UP));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::DOWN));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::LEFT));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::RIGHT));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_INSERT))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::INS));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DELETE))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::DEL));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGUP))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::PGUP));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGDOWN))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::PGDOWN));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD1))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD1));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD2))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD2));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD3))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD3));
        if (!InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_NUMPAD5))
            _keys = ~(~(_keys) | static_cast<size_t>(keyPressed_flags::NUMPAD5));
            
    }
}

void PlayState::reposicionaCamara()
{
    switch(_vista)
    {
        case camara_view::SEMICENITAL:  _camera->setPosition(_vCarsRayCast[_cursorVehiculo]->getPosicionActual().x,
                                                             _vCarsRayCast[_cursorVehiculo]->getPosicionActual().y +10 ,
                                                             _vCarsRayCast[_cursorVehiculo]->getPosicionActual().z + 30);  break;
        case camara_view::TRASERA_BAJA: break;
        case camara_view::TRASERA_ALTA: break;
        case camara_view::INTERIOR:     break;
        default: assert(true);                                                            

    }
    
}

void PlayState::colocaCamara()
{
    cout << "padre de atacheo de camara: " << _camera->getParentNode() << endl;
    switch (_vista)
    {
        case camara_view::SEMICENITAL:  {_nodoVista->detachObject(_camera);
                                        _vCarsRayCast[_cursorVehiculo]->getSceneNode()->removeChild(_nodoVista);
                                        nodoCamera_t cam = SceneNodeConfig::getSingleton().getInfoCamera("IntroCamera");
                                        _camera->setPosition(cam.posInicial);
        _camera->lookAt(_vCarsRayCast[_cursorVehiculo]->getPosicionActual()); }
                                        break;
        case camara_view::TRASERA_BAJA: _camera->setPosition(_vCarsRayCast[_cursorVehiculo]->getPosicionActual().x,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().y + 3,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().z - 5); 
                                                      _camera->lookAt(_vCarsRayCast[_cursorVehiculo]->getPosicionActual()); break;
        case camara_view::TRASERA_ALTA: _camera->setPosition(_vCarsRayCast[_cursorVehiculo]->getPosicionActual().x,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().y + 5,
                                                      _vCarsRayCast[_cursorVehiculo]->getPosicionActual().z - 5); 
                                                      _camera->lookAt(_vCarsRayCast[_cursorVehiculo]->getPosicionActual()); break;
        case camara_view::INTERIOR:     _nodoVista->attachObject(_camera);
                                        _vCarsRayCast[_cursorVehiculo]->getSceneNode()->addChild(_nodoVista);
//                                      _camera->setPosition(0,0.13,-0.15); // Camara interior
                                        _camera->setPosition(0,1.5,-3);
                                        //_camera->yaw(Ogre::Degree(180));

        case camara_view::TOTAL_COUNT:                                                
        default: assert(true);
    }   
    
                            
}

bool PlayState::mouseMoved(const OIS::MouseEvent &e) { return true; }
bool PlayState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) { return true; }
bool PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) { return true; }
PlayState *PlayState::getSingletonPtr() { return msSingleton; }
PlayState &PlayState::getSingleton() { assert(msSingleton); return *msSingleton; }
void PlayState::createMyGui() {}
void PlayState::destroyMyGui() {}
