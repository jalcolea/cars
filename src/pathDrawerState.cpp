#include "pathDrawerState.h"

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

template<> pathDrawerState *Ogre::Singleton<pathDrawerState>::msSingleton = 0;

void pathDrawerState::enter()
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


    //Color de fondo inicial
    _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));

    // Cargar los parámetros para construir elementos del juego (coches, circuitos, camaras, etc)
    cargarParametros("SceneNodes.xml",true);
    
    // Configurar camara
    configurarCamaraPrincipal();

    //Preparar escena
    createScene();
    
    _exitGame = false;
    _deltaT = 0;
    sounds::getInstance()->play_music("begin");

}

void pathDrawerState::exit()
{
}

void pathDrawerState::pause()
{
}

void pathDrawerState::resume()
{
}

bool pathDrawerState::keyPressed(const OIS::KeyEvent& e)
{
    if (e.key == OIS::KC_ESCAPE)
        _exitGame = true;
        
    
    return true;
}

bool pathDrawerState::keyReleased(const OIS::KeyEvent& e)
{
        return true;
}

bool pathDrawerState::mouseMoved(const OIS::MouseEvent& e)
{
        return true;
}

bool pathDrawerState::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
        
    if (id == OIS::MB_Left)
        _crearMarca = true;
    
    return true;
}

bool pathDrawerState::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
        return true;
}

bool pathDrawerState::frameStarted(const Ogre::FrameEvent& evt)
{
    _deltaT = evt.timeSinceLastFrame;
    Vector3 vt = Vector3::ZERO;
    Real speed = 10.0;
    Real zoom = 0;
    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP))
        vt += Vector3(0,1,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN))
        vt += Vector3(0,-1,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT))
        vt += Vector3(-1,0,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
        vt += Vector3(1,0,0);
        
    // Zoom de la cámara con rueda ratón. Con teclado más suave.
    zoom += -100 * _deltaT * InputManager_::getSingletonPtr()->getMouse()->getMouseState().Z.rel * speed;   
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGDOWN))
        zoom += 1;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGUP))
        zoom -= 1;

    _camera->setOrthoWindowHeight(_camera->getOrthoWindowHeight() + zoom * _deltaT * speed);
    _camera->moveRelative(vt * _deltaT * speed);
    
    // Si hemos presionado MouseButtonLeft flag _crearMarca ON
    if (_crearMarca)
    {
        if (_nodoSelector)
            _nodoSelector->showBoundingBox(false);

        // TODO AJUSTAR MÁSCARAS PARA EL RAYO, DE MODO QUE PODAMOS SELECCIONAR UNA MARCA Y MOVERLA
        
        int posx = InputManager_::getSingletonPtr()->getMouse()->getMouseState().X.abs;
        int posy = InputManager_::getSingletonPtr()->getMouse()->getMouseState().Y.abs;
        Ray r = setRayQuery(posx, posy, MASK_CIRCUITO | MASK_MARCA);
        RaySceneQueryResult &result = _raySceneQuery->execute();
        RaySceneQueryResult::iterator it;
        it = result.begin();

        if (it != result.end()) 
        {
                if (it->movable->getParentSceneNode()->getName() == "track1bis") 
                {
                  marquita marca;
                  marca._nombreNodo = "nodoMarca" + to_string(_idMarca);
                  marca._nodoMarca = _sceneMgr->createSceneNode(marca._nombreNodo);
                  marca._nombreEnt = "entMarca" + to_string(_idMarca);                  
                  marca._entMarca = _sceneMgr->createEntity(marca._nombreEnt,"marca.mesh");
                  marca._entMarca->setCastShadows(false);
                  marca._entMarca->setQueryFlags(MASK_MARCA);
                  marca._nodoMarca->attachObject(marca._entMarca);
                  marca._nodoMarca->translate(r.getPoint(it->distance));
                  _sceneMgr->getRootSceneNode()->addChild(marca._nodoMarca);
                  marca._id = _idMarca;
                  _idMarca++;
                  
                  vMarcas.push_back(marca);
                  
                  if (marca._id > 0) dibujaLinea();
                  
                  cout << "nombre nodo marca creado: " << marca._nodoMarca->getName() << endl;
                  cout << "nombre entity marca creado: " << marca._entMarca->getName() << endl;
                  cout << "posicion de la marca creada: " << marca._nodoMarca->getPosition() << endl;
                }
                
            _nodoSelector = it->movable->getParentSceneNode();
            _nodoSelector->showBoundingBox(true);
            _nodoSelector->setDebugDisplayEnabled(true);
            cout << _nodoSelector->getName() << endl;
        }
        
        _crearMarca = false;
    }
    
    return !_exitGame;
}

void pathDrawerState::dibujaLinea()
{
    // create ManualObject
    ManualObject* manual = _sceneMgr->createManualObject("line" + to_string(_idMarca-2));
     
    manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
        // define start and end point
        manual->position(vMarcas[_idMarca-2]._nodoMarca->getPosition()); //start
        manual->position(vMarcas[_idMarca-1]._nodoMarca->getPosition());    //end
    manual->end();
     
    // add ManualObject to the RootSceneNode (so it will be visible)
    _sceneMgr->getRootSceneNode()->attachObject(manual);
}

bool pathDrawerState::frameEnded(const Ogre::FrameEvent& evt)
{
    
    return !_exitGame;
}

bool pathDrawerState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent& e)
{
    return true;
}

bool pathDrawerState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent& e)
{
    return true;    
}

bool pathDrawerState::WiimoteIRMove(const wiimWrapper::WiimoteEvent& e)
{
    return true;    
}

pathDrawerState& pathDrawerState::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}

pathDrawerState* pathDrawerState::getSingletonPtr()
{
    return msSingleton;
}

void pathDrawerState::createLight()
{
    _sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
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

void pathDrawerState::createMyGui()
{
}

void pathDrawerState::destroyMyGui()
{
}

void pathDrawerState::createScene()
{
//    _sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    _sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
    _sceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
    _sceneMgr->setShadowFarDistance(100);
    _sceneMgr->setSkyBox(true, "skybox");

    //createOverlay();
    createLight();
    //createFloor();
    //createMyGui();
    
    nodoOgre_t info =  SceneNodeConfig::getSingleton().getInfoNodoOgre("track1bis");
    Entity* entTrack = _sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla);
    entTrack->setQueryFlags(1);
    _track = _sceneMgr->createSceneNode(info.nombreNodo);
    _track->attachObject(entTrack);
    _sceneMgr->getRootSceneNode()->addChild(_track);
    _track->setPosition(0,0,0);
    _track->scale(1.5,1.5,1.5);
    //_track->pitch(Ogre::Degree(90));

    _nodoSelector = nullptr;
    _raySceneQuery = _sceneMgr->createRayQuery(Ray());
    _idMarca = 0;
    _crearMarca = false;
}


void pathDrawerState::createFloor()
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
    floorNode->setPosition(Vector3(0, 2, 0));
}

void pathDrawerState::cargarParametros(string archivo, bool consoleOut)
{
    //Cargamos la info para generar elementos de la escena (scenenodes, cameras, lights)
    SceneNodeConfig::getSingleton().load_xml("SceneNodes.xml");
    
    map_nodos_t nodos = SceneNodeConfig::getSingleton().getMapNodos();
    map_cameras_t camaras = SceneNodeConfig::getSingleton().getMapCameras();
    
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
    }

}

void pathDrawerState::configurarCamaraPrincipal()
{
    //Configuramos la camara
//    double width = _viewport->getActualWidth();
//    double height = _viewport->getActualHeight();
//    nodoCamera_t cam = SceneNodeConfig::getSingleton().getInfoCamera("IntroCamera");
//    _camera->setAspectRatio(width / height);

    //_camera->setPosition (Vector3 (-6.6,14.8659,-20));
    _camera->setPosition (Vector3 (0,15,0));
    _camera->lookAt (Vector3 (0,0,0.1));
    _camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    _camera->setOrthoWindowHeight(40);
    //_camera->setOrthoWindowWidth(15);
    
//    _camera->setPosition(cam.posInicial);
//    _camera->lookAt(Vector3(0,0,0));
//    _camera->setNearClipDistance(cam.nearClipDistance);
//    _camera->setFarClipDistance(cam.farClipDistance);
}

Ray pathDrawerState::setRayQuery(int posx, int posy, uint32 mask) {
  Ray rayMouse = _camera->getCameraToViewportRay(posx/float(_viewport->getActualWidth()), posy/float(_viewport->getActualHeight()));
  _raySceneQuery->setRay(rayMouse);
  _raySceneQuery->setSortByDistance(true);
  _raySceneQuery->setQueryMask(mask);
  return (rayMouse);
}