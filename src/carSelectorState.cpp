#include "carSelectorState.h"
#include "PlayState.h"
#include "actualOptions.h"
//#include <limits>

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

template<> carSelectorState *Ogre::Singleton<carSelectorState>::msSingleton = 0;

void carSelectorState::enter()
{
    difficult = 1;
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

  destroyMyGui();
  // Preparar bundle con las opciones seleccionadas.
  actualOptions::getSingletonPtr()->setDificultad(difficult);
  actualOptions::getSingletonPtr()->setIdNombreVehiculoXML(getNombreTipoCocheSeleccionado());
  actualOptions::getSingletonPtr()->setIdMaterialActual(_idMaterialActual);
  
  _sceneMgr->clearScene();

  
}

void carSelectorState::pause()
{
}

void carSelectorState::resume()
{
}

bool carSelectorState::keyPressed(const OIS::KeyEvent& e)
{
        
    if (!_girandoRuleta)
    {
        if (e.key == OIS::KC_LEFT)
        {
            sounds::getInstance()->play_effect("push");
            _sentidoGiro = 1;
            inicializarEstadoRotacionSelector(1.0f/120, _nodoSelector->getOrientation(),60,Vector3::UNIT_Y,_sentidoGiro);
            _girandoRuleta = true;
        }
        else if (e.key == OIS::KC_RIGHT)
        {
            sounds::getInstance()->play_effect("push");
            _sentidoGiro = -1;
            inicializarEstadoRotacionSelector(1.0f/120, _nodoSelector->getOrientation(),60,Vector3::UNIT_Y,_sentidoGiro);
            _girandoRuleta = true;
        }
        
        //sincronizarIdMaterialConVehiculoSeleccionado();
    }

    if (e.key == OIS::KC_C)
    {
        sounds::getInstance()->play_effect("push");
        cambiarMaterialVehicSeleccionado();
    }

    else if ((e.key == OIS::KC_RETURN) || (e.key == OIS::KC_P))
    {
        sounds::getInstance()->play_effect("push");
        changeState(PlayState::getSingletonPtr());
    }
    else if (e.key == OIS::KC_E || e.key == OIS::KC_ESCAPE)
         popState();
//    else if (e.key==OIS::KC_0) setDifficult (0);
//    else if (e.key==OIS::KC_1) setDifficult (1);
//    else if (e.key==OIS::KC_2) setDifficult (2);


//        _exitGame = true;

    
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
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  char tmp [32];

  if (btn_back->_checkPoint(x,y))
  {
    sounds::getInstance()->play_effect("push");
    popState();
  }
  if (btn_mat->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_C,0);
    carSelectorState::keyPressed(e);
  }
  else if (btn_left->_checkPoint(x,y)) 
  {
    OIS::KeyEvent e(NULL,OIS::KC_LEFT,0);
    carSelectorState::keyPressed(e);
  }
  else if (btn_right->_checkPoint(x,y)) 
  {
    OIS::KeyEvent e(NULL,OIS::KC_RIGHT,0);
    carSelectorState::keyPressed(e);
  }
  else if (btn_play->_checkPoint(x,y))
  {
    OIS::KeyEvent e(NULL,OIS::KC_RETURN,0);
    carSelectorState::keyPressed(e);
  }
/*
  else if (btn_d1->_checkPoint(x,y))
  {
    difficult = 1;
    sounds::getInstance()->play_effect("push");
    for (int a=0;a<3;a++) img[a]->setVisible(false);
    img[difficult-1]->setVisible(true);
  }
  else if (btn_d2->_checkPoint(x,y))
  {
    difficult = 2;
    sounds::getInstance()->play_effect("push");
    for (int a=0;a<3;a++) img[a]->setVisible(false);
    img[difficult-1]->setVisible(true);
  }
  else if (btn_d3->_checkPoint(x,y))
  {
    difficult = 3;
    sounds::getInstance()->play_effect("push");
    for (int a=0;a<3;a++) img[a]->setVisible(false);
    img[difficult-1]->setVisible(true);
  }
*/
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
        sincronizarIdMaterialConVehiculoSeleccionado();
        setDifficult(_vDificultad[_cursorVehiculo]);
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
    Light *light = _sceneMgr->createLight("LightCarSelector");
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
  layout = MyGUI::LayoutManager::getInstance().loadLayout(LAYOUT_OPTIONS);
  btn_back = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_back");
  btn_left = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_left");
  btn_right = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_right");
  btn_play = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_play");
  btn_mat = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_mat");
  //btn_d1 = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_d1");
  //btn_d2 = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_d2");
  //btn_d3 = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_d3");
  text = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("text");
  edt_diff = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("edt_diff");
  //img[0] = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_d1");
  //img[1] = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_d2");
  //img[2] = MyGUI::Gui::getInstance().findWidget<MyGUI::ImageBox>("img_d3");
}

void carSelectorState::destroyMyGui()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
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
    createMyGui();
    
    nodoOgre_t info =  SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("track1bis");
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
    

    // Ojo con esto, hay que limpiar los vectores para que no queden posibles referencias colgando.
    // Pues al salir y volver a entrar en este estado podrían estar activas todavía.
    _vEntCars.clear();
    _vCars.clear();
    
    info = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("kartOneBlock");
    _vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    _vDificultad.push_back(info.dificultad);
    info = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("farara-sportOneBlock");
    _vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    _vDificultad.push_back(info.dificultad);
    info = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("formulaOneBlock");
    _vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    _vDificultad.push_back(info.dificultad);
    info = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("groupC1OneBlock");
    _vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    _vDificultad.push_back(info.dificultad);
    info = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("lamba-sportOneBlock");
    _vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    _vDificultad.push_back(info.dificultad);
    info = SceneNodeConfig::getSingletonPtr()->getInfoNodoOgre("parsche-sportOneBlock");
    _vEntCars.push_back(_sceneMgr->createEntity(info.nombreEntidad,info.nombreMalla));
    _vDificultad.push_back(info.dificultad);

    SceneNode* aux;
    for (size_t i = 0; i != _vEntCars.size(); ++i)
    {
        cout << _vEntCars[i] << endl;
        int anguloOffset = 90; // desplazamos 90 grados para que el primer coche lo ponga justo en (0,-1,0), o sea, que quede en frente nuestra y seleccionado.
        int angulo = i * 60; // En realidad hay 7 coches distintos pero 2 de ellos son prácticamente iguales así que nos quedamos con 6
        aux = _nodoSelector->createChildSceneNode("nodoCar"+i,Vector3(Ogre::Math::Cos(Ogre::Degree(angulo + anguloOffset)), 0, Ogre::Math::Sin(Ogre::Degree(angulo+anguloOffset))));
        aux->attachObject(_vEntCars[i]);
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
    //_progresoBajada = MIN_ALTURA_SELECCIONADO;
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
    SceneNode *floorNode = _sceneMgr->createSceneNode("floorCarSelector");
    Plane planeFloor;
    planeFloor.normal = Vector3(0, 1, 0);
    planeFloor.d = 2;
    MeshManager::getSingleton().createPlane("FloorPlane", 
                                          ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                          planeFloor, 200000, 200000, 20, 20, true, 1, 9000, 9000, Vector3::UNIT_Z);
    Entity *entFloor = _sceneMgr->createEntity("floorCarSelector", "FloorPlane");
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
    //_camera->setPosition(cam.posInicial);
    _camera->setPosition(0,16,28);
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

string carSelectorState::getNombreTipoCocheSeleccionado()
{
    return _vEntCars[_cursorVehiculo]->getName().substr(0,_vEntCars[_cursorVehiculo]->getName().find("OneBlock"));
}

void carSelectorState::sincronizarIdMaterialConVehiculoSeleccionado()
{
    string aux = static_cast<Entity *>(_vCars[_cursorVehiculo]->getAttachedObject(0))->getSubEntity(0)->getMaterialName();
    size_t i;
    for (i=0; i<_vMateriales.size(); i++)
        if (_vMateriales[i] == aux)
        {
            _idMaterialActual = i;
            break;
        }
        
    cout << "El vehículo " << _cursorVehiculo << " tiene el material " << _vMateriales[i] << " con id " << i << endl;
    
}

void carSelectorState::setDifficult (int level)
{
    cout << "SET DIFF " << level <<endl;
    cout << "SET DIFF " << level << " " << msg_diff[level]<<endl;
    edt_diff->setCaption(msg_diff[level]);
}
