#include "carSelectorState.h"
#include <limits>

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

template<> carSelectorState *Ogre::Singleton<carSelectorState>::msSingleton = 0;

void carSelectorState::enter()
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
    
    _girandoRuleta = false;
    _sentidoGiro = -1;
    _exitGame = false;
    _deltaT = 0;
    sounds::getInstance()->play_music("begin");

}

void carSelectorState::exit()
{
}

void carSelectorState::pause()
{
}

void carSelectorState::resume()
{
}

bool carSelectorState::keyPressed(const OIS::KeyEvent& e)
{
    if (e.key == OIS::KC_ESCAPE)
        _exitGame = true;
        
    if (!_girandoRuleta)
    {
        if (e.key == OIS::KC_LEFT)
        {
            _girandoRuleta = true;
            _sentidoGiro = -1;
        }
        else if (e.key == OIS::KC_RIGHT)
        {
            _girandoRuleta = true;
            _sentidoGiro = 1;
        }
    }    
    
    return true;
}

bool carSelectorState::keyReleased(const OIS::KeyEvent& e)
{
        return true;
}

bool carSelectorState::mouseMoved(const OIS::MouseEvent& e)
{
        return true;
}

bool carSelectorState::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
        return true;
}

bool carSelectorState::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
        return true;
}

bool carSelectorState::frameStarted(const Ogre::FrameEvent& evt)
{
    Real rotSeleccionado = 0;
    Real rotSelector = 0;
    
    _deltaT = evt.timeSinceLastFrame;
    
    if (!_girandoRuleta)
    {
        rotSeleccionado-=180;
        _vCars[_cursorVehiculo]->yaw(Ogre::Radian(rotSeleccionado * _deltaT * 0.005));
    }
    else
    {
        rotSelector += 360;
        _nodoSelector->yaw(Ogre::Radian(rotSelector * _deltaT * 0.005));
        if ((Ogre::Degree(_nodoSelector->getOrientation().getYaw(true))  >= Ogre::Degree(60)) ||
           (Ogre::Degree(_nodoSelector->getOrientation().getYaw(true)) * Ogre::Degree(_cursorVehiculo+1) <= Ogre::Degree(-60)))
        {
            cout <<"grados: " << Ogre::Degree(_nodoSelector->getOrientation().getYaw(true)) << endl;
            _girandoRuleta = false;
            _cursorVehiculo++;
            _cursorVehiculo = _cursorVehiculo % _vCars.size();
        }
        
        
    }
    
    return !_exitGame;
}

bool carSelectorState::frameEnded(const Ogre::FrameEvent& evt)
{
    
    return !_exitGame;
}

bool carSelectorState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent& e)
{
    return true;
}

bool carSelectorState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent& e)
{
    return true;    
}

bool carSelectorState::WiimoteIRMove(const wiimWrapper::WiimoteEvent& e)
{
    return true;    
}

carSelectorState& carSelectorState::getSingleton()
{
    assert(msSingleton);
    return *msSingleton;
}

carSelectorState* carSelectorState::getSingletonPtr()
{
    return msSingleton;
}

void carSelectorState::createLight()
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

void carSelectorState::createMyGui()
{
}

void carSelectorState::destroyMyGui()
{
}

void carSelectorState::createScene()
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
    
    nodoOgre_t info =  SceneNodeConfig::getSingleton().getInfoNodoOgre("track1bis");
    Entity* entTrack = _sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla);
    _track = _sceneMgr->createSceneNode(info.nombreNodo);
    _track->attachObject(entTrack);
    _sceneMgr->getRootSceneNode()->addChild(_track);
    _track->setPosition(0,0,0);

    _nodoSelector = _sceneMgr->createSceneNode("nodoSelector");
    Entity* entSelector = _sceneMgr->createEntity("entSelector","PlanoSelector.mesh");
    _nodoSelector->attachObject(entSelector);
    _sceneMgr->getRootSceneNode()->addChild(_nodoSelector);
    _nodoSelector->scale(4,4,4);
    _nodoSelector->setPosition(0,12,20);
    //_nodoSelector->yaw(Ogre::Degree(-90));
    //_nodoSelector->roll(Ogre::Degree(10));
    

    std::vector<Entity*> vEntCars;
    info = SceneNodeConfig::getSingleton().getInfoNodoOgre("kartOneBlock");
    vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    info = SceneNodeConfig::getSingleton().getInfoNodoOgre("farara-sportOneBlock");
    vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    info = SceneNodeConfig::getSingleton().getInfoNodoOgre("formulaOneBlock");
    vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    info = SceneNodeConfig::getSingleton().getInfoNodoOgre("groupC1OneBlock");
    vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    info = SceneNodeConfig::getSingleton().getInfoNodoOgre("lamba-sportOneBlock");
    vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    info = SceneNodeConfig::getSingleton().getInfoNodoOgre("parsche-sportOneBlock");
    vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));

    SceneNode* aux;
    for (size_t i = 0; i != vEntCars.size(); ++i)
    {
        int angulo = i * 60; // En realidad hay 7 coches distintos pero 2 de ellos son prácticamente iguales así que nos quedamos con 6
        aux = _nodoSelector->createChildSceneNode("nodoCar"+i,Vector3(Ogre::Math::Cos(Ogre::Degree(angulo)),0,Ogre::Math::Sin(Ogre::Degree(angulo))));
        aux->attachObject(vEntCars[i]);
        aux->scale(0.2,0.2,0.2);
        aux->yaw(Ogre::Degree(30));
        _vCars.push_back(aux);
    }
    
    _cursorVehiculo = 0;
    
    _vCars[_cursorVehiculo]->scale(4,4,4);
    
}

void carSelectorState::createFloor()
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

void carSelectorState::cargarParametros(string archivo, bool consoleOut)
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

void carSelectorState::configurarCamaraPrincipal()
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

void carSelectorState::colocaCamara()
{
}

void carSelectorState::gestionaAnimaciones(Ogre::AnimationState*& anim, Ogre::Real deltaT, const String& nombreEnt, const String& nombreAnim)
{
}

void carSelectorState::createOverlay()
{
}

void carSelectorState::pintaOverlayInfo()
{
}

void carSelectorState::flagKeys(bool flag)
{
}
