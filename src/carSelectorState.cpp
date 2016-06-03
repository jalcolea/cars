#include "carSelectorState.h"
//#include <limits>

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
            _sentidoGiro = 1;
            inicializarEstadoRotacionSelector(1.0f/120, _nodoSelector->getOrientation(),60,Vector3::UNIT_Y,_sentidoGiro);
            _girandoRuleta = true;
        }
        else if (e.key == OIS::KC_RIGHT)
        {
            _sentidoGiro = -1;
            inicializarEstadoRotacionSelector(1.0f/120, _nodoSelector->getOrientation(),60,Vector3::UNIT_Y,_sentidoGiro);
            _girandoRuleta = true;
        }
    }

    if (e.key == OIS::KC_M)
        cambiarMaterialVehicSeleccionado();
    
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
    Real speed = 0.005;
    
    _deltaT = evt.timeSinceLastFrame;
    
    if (!_girandoRuleta)
    {
        rotSeleccionado-=180;
        _vCars[_cursorVehiculo]->yaw(Ogre::Radian(rotSeleccionado * _deltaT * speed));
    }
    else
        girarRuleta();
    
    if (_subiendo)
        subirSeleccionado();
    
    if (_bajando)
        bajarDeseleccionado();
    
    return !_exitGame;
}

void carSelectorState::bajarDeseleccionado()
{
    Vector3 aux = _vCars[_idVehicBajando]->getPosition();
    if (_progresoBajada < MIN_ALTURA_SELECCIONADO)
    {
        _progresoBajada = MAX_ALTURA_SELECCIONADO;
        aux.y = MIN_ALTURA_SELECCIONADO;
        _bajando = false;
    }
    else
    {
        _progresoBajada -= 0.1 * _deltaT * SPEED_MOVIMIENTOS;
        aux.y = _progresoBajada;
    }

    _vCars[_idVehicBajando]->setPosition(aux.x,aux.y,aux.z);
    
}

void carSelectorState::subirSeleccionado()
{
    Vector3 aux = _vCars[_idVehicSubiendo]->getPosition();
    if (_progresoSubida > MAX_ALTURA_SELECCIONADO)
    {
        _progresoSubida = MIN_ALTURA_SELECCIONADO;
        aux.y = MAX_ALTURA_SELECCIONADO;
        _subiendo = false;
    }
    else
    {
        _progresoSubida += 0.1 * _deltaT * SPEED_MOVIMIENTOS;
        aux.y = _progresoSubida;
    }

    _vCars[_idVehicSubiendo]->setPosition(aux.x,aux.y,aux.z);

}


void carSelectorState::girarRuleta()
{
    if (_girandoRuleta)
    {
        _progresoRotacion += _factorRotacion;
        if (_progresoRotacion > 1)
        {
            inicializarEstadoRotacionSelector(1.0f/120, _nodoSelector->getOrientation(),60,Vector3::UNIT_Y,_sentidoGiro);
            //Vector3 pos = _vCars[_cursorVehiculo]->getPosition();
            //_vCars[_cursorVehiculo]->setPosition(pos.x,0,pos.z);
            _vCars[_cursorVehiculo]->setScale(Vector3(0.2,0.2,0.2));
            
            _idVehicBajando = _cursorVehiculo;
            _bajando = true;
            
            _cursorVehiculo += _sentidoGiro; // siguiente vehiculo a seleccionar. _sentidoGiro puede valer -1 ó 1, luego nos sirve para aumentar/decrementar el cursor de vehículos
            if (_cursorVehiculo < 0) _cursorVehiculo += _vCars.size(); // Si el indice resultante es negativo complementamos, para que el indice esté en rango(0-5)
            _cursorVehiculo = abs(_cursorVehiculo) % _vCars.size(); // modulamos para recorrer los vehiculos del vector 
            
            _vCars[_cursorVehiculo]->scale(Vector3(2,2,2));
            
            _idVehicSubiendo = _cursorVehiculo;
            _subiendo = true;
            
            //pos = _vCars[_cursorVehiculo]->getPosition();
            //_vCars[_cursorVehiculo]->setPosition(pos.x,pos.y + 0.5,pos.z);
            
        }
        else
        {
            Quaternion delta = Quaternion::Slerp(_progresoRotacion, _orientOriginal, _orientDestino, true);
            _nodoSelector->setOrientation(delta);
        }
    }
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
    _nodoSelector->scale(3.5,3.5,3.5);
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
        int anguloOffset = 90; // desplazamos 90 grados para que el primer coche lo ponga justo en (0,-1,0), o sea, que quede en frente nuestra y seleccionado.
        int angulo = i * 60; // En realidad hay 7 coches distintos pero 2 de ellos son prácticamente iguales así que nos quedamos con 6
        aux = _nodoSelector->createChildSceneNode("nodoCar"+i,Vector3(Ogre::Math::Cos(Ogre::Degree(angulo + anguloOffset)), 0, Ogre::Math::Sin(Ogre::Degree(angulo+anguloOffset))));
        aux->attachObject(vEntCars[i]);
        aux->setScale(0.2,0.2,0.2);
        aux->yaw(Ogre::Degree(30));
        _vCars.push_back(aux);
    }
    
    _cursorVehiculo = 0;
    _vCars[_cursorVehiculo]->scale(2,2,2);
    Vector3 pos = _vCars[_cursorVehiculo]->getPosition();
    _vCars[_cursorVehiculo]->setPosition(Vector3(pos.x,pos.y+0.5,pos.z));
    
    _sentidoGiro = 1;
    _bajando = false;
    _subiendo = false;
    _progresoBajada = MAX_ALTURA_SELECCIONADO;
    _progresoBajada = MIN_ALTURA_SELECCIONADO;
    _idMaterialActual = 0;

    
    inicializarEstadoRotacionSelector(1.0f/120, _nodoSelector->getOrientation(),60,Vector3::UNIT_Y,1);

}

// Procedimiento por comidad para reinicializar los parámetros de la rotacion Slerp del plano selector de coches.
void carSelectorState::inicializarEstadoRotacionSelector(Real factorRot, Quaternion orientacionOriginal, Real angulo, Vector3 eje, int sentido)
{
    _girandoRuleta = false;
    _orientOriginal = orientacionOriginal;
    _factorRotacion = factorRot;
    Quaternion q = Quaternion(Degree(angulo * sentido),eje);
    _orientDestino = Quaternion(q * _orientOriginal);
    _progresoRotacion = 0;
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

void carSelectorState::cambiarMaterialVehicSeleccionado()
{
    _idMaterialActual++;
    _idMaterialActual = _idMaterialActual % _vMateriales.size();
    
    // NOTA: getSubEntity(0)  El índice de getSubEntity, en este caso 0, se establece cuando exportamos el modelo desde Blender, 
    // el exportador establece el indice del submesh según el orden de los materiales que use el modelo en Blender. En esta parte del juego
    // queremos cambiar el material del chasis del vehiculo. Es decir, de la parte del modelo más grande. Si esa parte del modelo en 
    // blender usa un material cuyo índice no sea 0, es decir el primero, el exportador le asignará a esa parte del modelo (el submesh)
    // el índice que tenga el material en Blender. Luego si hardcodeamos el índice del subEntity, como en este caso, tendremos que tener
    // en cuenta este detalle, de lo contrario estaríamos intentando cambiar el material al subEntity (submesh) erróneo.
    static_cast<Entity *>(_vCars[_cursorVehiculo]->getAttachedObject(0))->getSubEntity(0)->setMaterialName(_vMateriales[_idMaterialActual]);

}

