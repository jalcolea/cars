#include "pathDrawerState.h"
#include "IAPointsSerializer.h"
#include "IAPointsDeserializer.h"

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
        
    if (e.key == OIS::KC_F2)
        guardarRuta();
        
    if (e.key == OIS::KC_F4)
        cargarRutaCheckPoint("rutasIA.xml");
        //cargarRuta("rutasIA.xml");
        
    if (e.key == OIS::KC_F9)
        borrarTodasLasMarcas();
        
    if (e.key == OIS::KC_U &&
        _nodoSelector && 
        _nodoSelector->getName().substr(0,_nodoSelector->getName().find("_")-1) != _checkPointInfo.nombreNodo)
    {
        Quaternion q = Quaternion::IDENTITY;
        q.FromAngleAxis(Ogre::Degree(-90),Vector3::UNIT_Y);
        _nodoSelector->setOrientation(q);
    }
        
    
    return true;
}

void pathDrawerState::guardarRuta()
{
    IAPointsSerializer iaps;
    
    iaps.nuevoXMLIAPoints();
    for (size_t i = 0; i < vMarcas.size(); i++)
    {
//        iapoint p;
//        p.x(vMarcas[i]._nodoMarca->getPosition().x);
//        p.y(vMarcas[i]._nodoMarca->getPosition().y);
//        p.z(vMarcas[i]._nodoMarca->getPosition().z);
        punto point;
        point.p.x = vMarcas[i]._nodoMarca->getPosition().x;
        point.p.y = vMarcas[i]._nodoMarca->getPosition().y;
        point.p.z = vMarcas[i]._nodoMarca->getPosition().z;

        iaps.addNodoXMLIAPoints(i,point,vMarcas[i]._nodoMarca->getOrientation());
    }    
    
    iaps.guardarXMLIAPoints("rutasIA.xml");
    cout << "FICHERO RUTAS GUARDADO" << endl;
}

void pathDrawerState::cargarRuta(string fichero)
{
    borrarTodasLasMarcas();
    
    IAPointsDeserializer iapd;
    iapd.cargarFichero(fichero);
//    std::vector<iapoint> vpoints = iapd.getPoints();
    std::vector<punto> vpoints = iapd.getPoints();
    
    for (size_t i = 0; i < vpoints.size(); i++)
    {
        //Vector3 pos(vpoints[i].x(),vpoints[i].y(),vpoints[i].z());
        Vector3 pos(vpoints[i].p); 
        addMarca(pos);
    }    
    
}

void pathDrawerState::cargarRutaCheckPoint(string fichero)
{
    borrarTodasLasMarcas();
    
    IAPointsDeserializer iapd;
    iapd.cargarFicheroCheckPoint(fichero);
    std::vector<CheckPoint> vpoints = iapd.getCheckPoints();
    
    for (size_t i = 0; i < vpoints.size(); i++)
    {
        Vector3 pos(vpoints[i].p.p.x,vpoints[i].p.p.y,vpoints[i].p.p.z);
        Quaternion q = vpoints[i].quat;
        addCheckPoint(pos,q);
    }    
    
}

void pathDrawerState::borrarTodasLasMarcas()
{
    _sceneMgr->destroyAllManualObjects();
    _sceneMgr->clearScene();
    createScene();
    vMarcas.clear();
}

bool pathDrawerState::keyReleased(const OIS::KeyEvent& e)
{
        return true;
}

bool pathDrawerState::mouseMoved(const OIS::MouseEvent& e)
{
    //cout << _nodoSelector->getName().substr(0,_nodoSelector->getName().find("_")) << endl;
    //cout << "mousemove " << _nodoSelector->getName() << endl;
    if (InputManager_::getSingletonPtr()->getMouse()->getMouseState().buttonDown(OIS::MB_Left) &&
        //_nodoSelector && _nodoSelector->getName() != "PlaneRoadBig") //"track1Big")
        _nodoSelector && 
        _nodoSelector->getName().substr(0,_nodoSelector->getName().find("_")-1) != _checkPointInfo.nombreNodo)
    {
        Ray r = setRayQuery(e.state.X.abs, e.state.Y.abs, MASK_CIRCUITO | MASK_MARCA);
        RaySceneQueryResult &result = _raySceneQuery->execute();
        RaySceneQueryResult::iterator it;
        it = result.begin();
        if (it != result.end() && it->movable->getParentSceneNode()->getName() == "PlaneRoadBig") 
        {   
            Vector3 pos = r.getPoint(it->distance);
            pos.y = _planeRoadNode->getPosition().y + 1;
            _nodoSelector->setPosition(pos);
            recolocarLinea();
        }
    }
        
    
    return true;
}

void pathDrawerState::recolocarLinea()
{
    for (auto it=vMarcas.begin(); it!=vMarcas.end(); ++it)
    {
        if (_nodoSelector == (*it)._nodoMarca)
        {
            if ((*it)._id < vMarcas.size()-1) // Si el id del nodo NO es el último, entonces redibujo la linea que sale de él
            {
                _sceneMgr->destroyManualObject("line_" + to_string((*it)._id));
                dibujaLinea((*it)._id, (*it)._id+1);
            }

            if ((*it)._id) // Si el id del nodo es distinto de 0, entonces redibujo la linea que le llega.
            {
                _sceneMgr->destroyManualObject("line_" + to_string((*it)._id-1));
                dibujaLinea((*it)._id-1, (*it)._id);
            }
            return;
        }
    }
        
}

bool pathDrawerState::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
        
    if (id == OIS::MB_Left)
        _crearMarca = true;
        
    if (id == OIS::MB_Right)
        _rotarMarca = true;
    
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
    Vector3 vtCheck = Vector3::ZERO;
    Real speed = 10.0;
    Real zoom = 0;
    Real r = 0;
    Real rYaw = 0;
    Real rotCheckPoint = 0;
    
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP) &&
        !InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vt += Vector3(0,1,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN) &&
        !InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vt += Vector3(0,-1,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT) &&
        !InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vt += Vector3(-1,0,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT) &&
        !InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vt += Vector3(1,0,0);
        
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_UP) &&
        InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vtCheck += Vector3(0,0,-1);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DOWN) &&
        InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vtCheck += Vector3(0,0,1);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT) &&
        InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vtCheck += Vector3(-1,0,0);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT) &&
        InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LCONTROL))
        vtCheck += Vector3(1,0,0);

        
    // Zoom de la cámara con rueda ratón. Con teclado más suave.
    zoom += -100 * _deltaT * InputManager_::getSingletonPtr()->getMouse()->getMouseState().Z.rel * speed;   
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_INSERT))
        vt += Vector3(0,0,1);
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_DELETE))
        vt += Vector3(0,0,-1);

    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGUP)) r += 180;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_PGDOWN)) r += -180;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LSHIFT) &&
        InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LEFT)) rYaw += 180;
    if (InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LSHIFT) &&
        InputManager_::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_RIGHT)) rYaw += -180;


        
    _camera->pitch(Ogre::Radian(r * _deltaT * 0.005));
    _camera->yaw(Ogre::Radian(rYaw * _deltaT * 0.005));

    _camera->moveRelative(vt * _deltaT * speed);
    
    
    
    if (InputManager_::getSingletonPtr()->getMouse()->getMouseState().buttonDown(OIS::MB_Right)) rotCheckPoint += 180;
    if (_nodoSelector)
    {   
        _nodoSelector->yaw(Ogre::Radian(rotCheckPoint * _deltaT * 0.005));
        _nodoSelector->setPosition(_nodoSelector->getPosition() + vtCheck * _deltaT);
    }
    
    // Si hemos presionado MouseButtonLeft flag _crearMarca ON
    if (_crearMarca)
    {
        if (_nodoSelector)
            _nodoSelector->showBoundingBox(false);
        
        int posx = InputManager_::getSingletonPtr()->getMouse()->getMouseState().X.abs;
        int posy = InputManager_::getSingletonPtr()->getMouse()->getMouseState().Y.abs;
        Ray r = setRayQuery(posx, posy, MASK_CIRCUITO | MASK_MARCA);
        RaySceneQueryResult &result = _raySceneQuery->execute();
        RaySceneQueryResult::iterator it;
        it = result.begin();

        if (it != result.end()) 
        {       // Podríamos mejorar esto usando addMarca() pero vamos que nos vamos :D
                if (it->movable->getParentSceneNode()->getName() == "PlaneRoadBig") 
                {
                  marquita marca;
                  marca._nombreNodo =  _checkPointInfo.nombreNodo + "_" + to_string(_idMarca);
                  marca._nodoMarca = _sceneMgr->createSceneNode(marca._nombreNodo);
                  marca._nombreEnt = _checkPointInfo.nombreEntidad + "_" + to_string(_idMarca);                  
                  marca._entMarca = _sceneMgr->createEntity(marca._nombreEnt,_checkPointInfo.nombreMalla);
                  marca._entMarca->setCastShadows(false);
                  marca._entMarca->setQueryFlags(MASK_MARCA);
                  marca._nodoMarca->attachObject(marca._entMarca);
                  //marca._nodoMarca->translate(r.getPoint(it->distance));
                  Vector3 pos(r.getPoint(it->distance));
                  pos.y = _planeRoadNode->getPosition().y + 1;
                  marca._nodoMarca->setPosition(pos);
                  
                  _sceneMgr->getRootSceneNode()->addChild(marca._nodoMarca);
                  marca._id = _idMarca;
                  _idMarca++;
                  
                  vMarcas.push_back(marca);
                  
                  if (marca._id > 0) dibujaLinea(marca._id -1, marca._id);
                  
                  cout << "nombre nodo marca creado: " << marca._nodoMarca->getName() << endl;
                  cout << "nombre entity marca creado: " << marca._entMarca->getName() << endl;
                  cout << "posicion de la marca creada: " << marca._nodoMarca->getPosition() << endl;
                    
                    
                  /*marquita marca;
                  marca._nombreNodo = "nodoMarca_" + to_string(_idMarca);
                  marca._nodoMarca = _sceneMgr->createSceneNode(marca._nombreNodo);
                  marca._nombreEnt = "entMarca_" + to_string(_idMarca);                  
                  marca._entMarca = _sceneMgr->createEntity(marca._nombreEnt,"marca.mesh");
                  marca._entMarca->setCastShadows(false);
                  marca._entMarca->setQueryFlags(MASK_MARCA);
                  marca._nodoMarca->attachObject(marca._entMarca);
                  //marca._nodoMarca->translate(r.getPoint(it->distance));
                  Vector3 pos(r.getPoint(it->distance));
                  pos.y = _planeRoadNode->getPosition().y + 0.001;
                  marca._nodoMarca->setPosition(pos);
                  
                  _sceneMgr->getRootSceneNode()->addChild(marca._nodoMarca);
                  marca._id = _idMarca;
                  _idMarca++;
                  
                  vMarcas.push_back(marca);
                  
                  if (marca._id > 0) dibujaLinea(marca._id -1, marca._id);
                  
                  cout << "nombre nodo marca creado: " << marca._nodoMarca->getName() << endl;
                  cout << "nombre entity marca creado: " << marca._entMarca->getName() << endl;
                  cout << "posicion de la marca creada: " << marca._nodoMarca->getPosition() << endl;*/
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

void pathDrawerState::addMarca(Vector3 posicion)
{
      marquita marca;
      marca._nombreNodo = "nodoMarca_" + to_string(_idMarca);
      marca._nodoMarca = _sceneMgr->createSceneNode(marca._nombreNodo);
      marca._nombreEnt = "entMarca_" + to_string(_idMarca);                  
      marca._entMarca = _sceneMgr->createEntity(marca._nombreEnt,"marca.mesh");
      marca._entMarca->setCastShadows(false);
      marca._entMarca->setQueryFlags(MASK_MARCA);
      marca._nodoMarca->attachObject(marca._entMarca);
      marca._nodoMarca->translate(posicion);
      _sceneMgr->getRootSceneNode()->addChild(marca._nodoMarca);
      marca._id = _idMarca;
      _idMarca++;
      
      vMarcas.push_back(marca);
      
      if (marca._id > 0) dibujaLinea(marca._id -1, marca._id);
      
      cout << "nombre nodo marca creado: " << marca._nodoMarca->getName() << endl;
      cout << "nombre entity marca creado: " << marca._entMarca->getName() << endl;
      cout << "posicion de la marca creada: " << marca._nodoMarca->getPosition() << endl;

}

void pathDrawerState::addCheckPoint(Vector3 posicion, Quaternion q)
{
    marquita marca;
    marca._nombreNodo =  _checkPointInfo.nombreNodo + "_" + to_string(_idMarca);
    marca._nodoMarca = _sceneMgr->createSceneNode(marca._nombreNodo);
    marca._nombreEnt = _checkPointInfo.nombreEntidad + "_" + to_string(_idMarca);                  
    marca._entMarca = _sceneMgr->createEntity(marca._nombreEnt,_checkPointInfo.nombreMalla);
    marca._entMarca->setCastShadows(false);
    marca._entMarca->setQueryFlags(MASK_MARCA);
    marca._nodoMarca->attachObject(marca._entMarca);
    marca._nodoMarca->translate(posicion);
    _sceneMgr->getRootSceneNode()->addChild(marca._nodoMarca);
    marca.rotacion = q;
    marca._nodoMarca->setOrientation(q);
    
    marca._id = _idMarca;
    _idMarca++;

    vMarcas.push_back(marca);

    if (marca._id > 0) dibujaLinea(marca._id -1, marca._id);

    cout << "nombre nodo marca creado: " << marca._nodoMarca->getName() << endl;
    cout << "nombre entity marca creado: " << marca._entMarca->getName() << endl;
    cout << "posicion de la marca creada: " << marca._nodoMarca->getPosition() << endl;

}


void pathDrawerState::dibujaLinea(size_t idFrom, size_t idTo)
{
    ManualObject* manual = _sceneMgr->createManualObject("line_" + to_string(idFrom));
     
    manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_LIST);
        Vector3 aux(vMarcas[idFrom]._nodoMarca->getPosition());
        aux.y = _planeRoadNode->getPosition().y + 1;
        manual->position(aux); //start
        aux = vMarcas[idTo]._nodoMarca->getPosition();
        aux.y = _planeRoadNode->getPosition().y + 1;
        manual->position(aux);    //end
    manual->end();
     
    _sceneMgr->getRootSceneNode()->attachObject(manual);
    cout << "nombre entity linea: " << manual->getName() << endl;
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
    
    nodoOgre_t info =  SceneNodeConfig::getSingleton().getInfoNodoOgre("track1NoRoadBig");
    Entity* entTrack = _sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla);
    entTrack->setQueryFlags(MASK_ESCENARIO);
    _track = _sceneMgr->createSceneNode(info.nombreNodo);
    _track->attachObject(entTrack);
    _sceneMgr->getRootSceneNode()->addChild(_track);
    _track->setPosition(0,0,0);
    
    createPlaneRoad();

    _nodoSelector = nullptr;
    _raySceneQuery = _sceneMgr->createRayQuery(Ray());
    _idMarca = 0;
    _crearMarca = false;
    _rotarMarca = false;
    _checkPointInfo = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("CheckPointPlane");
}

void pathDrawerState::createPlaneRoad()
{
    nodoOgre_t nodoXML = SceneNodeConfig::getSingleton().getInfoNodoOgre("PlaneRoadBig");
    _planeRoadNode = _sceneMgr->createSceneNode(nodoXML.nombreNodo);
    Entity* planeRoadEnt = _sceneMgr->createEntity(nodoXML.nombreEntidad,nodoXML.nombreMalla);
    planeRoadEnt->setQueryFlags(MASK_CIRCUITO);
    planeRoadEnt->setCastShadows(true);
    _planeRoadNode->attachObject(planeRoadEnt);
    _sceneMgr->getRootSceneNode()->addChild(_planeRoadNode);
    _planeRoadNode->setPosition(nodoXML.posInicial);
    


   // PlaneRoadNode->setPosition(Vector3(0, 0, 0));
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
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    nodoCamera_t cam = SceneNodeConfig::getSingleton().getInfoCamera("IntroCamera");
    _camera->setAspectRatio(width / height);
    _camera->setPosition(cam.posInicial);
    _camera->lookAt(Vector3(0,0,0));
    _camera->setNearClipDistance(cam.nearClipDistance);
    _camera->setFarClipDistance(cam.farClipDistance);
}

Ray pathDrawerState::setRayQuery(int posx, int posy, uint32 mask) {
  Ray rayMouse = _camera->getCameraToViewportRay(posx/float(_viewport->getActualWidth()), posy/float(_viewport->getActualHeight()));
  _raySceneQuery->setRay(rayMouse);
  _raySceneQuery->setSortByDistance(true);
  _raySceneQuery->setQueryMask(mask);
  return (rayMouse);
}

