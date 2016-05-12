#include <Ogre.h>

#include "GameManager.h"
#include "GameState.h"

using namespace std;
using namespace Ogre;

template<> GameManager* Ogre::Singleton<GameManager>::msSingleton = 0;


GameManager::GameManager ()
{
  _root = 0;
}

GameManager::~GameManager ()
{
  while (!_states.empty()) {
    _states.top()->exit();
    _states.pop();
  }
  
  if (_root)
    delete _root;
}

void GameManager::start(GameState* state)
{
  // Creación del objeto Ogre::Root.
  _root = new Ogre::Root();
  
  //Creamos ya el _sceneManager, de lo contrario el OverlaySystem no se puede crear en este punto
  //Y es necesario crearlo antes de llamar a loadResources(), de lo contrario los scripts de overlays
  //los ignora por completo y no los carga. ADEMÁS: MIRAR COMENTARIO EN configure()
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
  //Creamos el OverlaySystem (DISCUTIR SI SERÍA MEJOR CONDICIONAR ESTO AL COMPILAR)
  _sceneManager->addRenderQueueListener(new Ogre::OverlaySystem());

  loadResources();

  if (!configure())
    return;    
    
  _inputMgr = new InputManager_;
  _inputMgr->initialise(_renderWindow);

  // Registro como key y mouse listener...
  _inputMgr->addKeyListener(this, "GameManager");
  _inputMgr->addMouseListener(this, "GameManager");
  
  // El GameManager es un FrameListener.
  _root->addFrameListener(this);

  // Transición al estado inicial.
  changeState(state);

  // Bucle de rendering.
  _root->startRendering();
}

bool GameManager::usarWiimote() 
{
  if (_inputMgr)
      if (_inputMgr->initialiseWiimote()) // Inicializar implica conectarse al wiimote. Si devuelve true es que nos hemos conectado.
      {
          cout << "wiimote conectado" << endl;
         _inputMgr->addWiimoteListener(this,"GameManager");
         return true;
	 }
  
  return false;

}

void GameManager::changeState(GameState* state)
{
  // Limpieza del estado actual.
  if (!_states.empty()) {
    // exit() sobre el último estado.
    _states.top()->exit();
    // Elimina el último estado.
    _states.pop();
  }

  // Transición al nuevo estado.
  _states.push(state);
  // enter() sobre el nuevo estado.
  _states.top()->enter();
}

void GameManager::pushState(GameState* state)
{
  // Pausa del estado actual.
  if (!_states.empty())
    _states.top()->pause();
  
  // Transición al nuevo estado.
  _states.push(state);
  // enter() sobre el nuevo estado.
  _states.top()->enter();
}

void GameManager::popState ()
{
  // Limpieza del estado actual.
  if (!_states.empty()) 
  {
    _states.top()->exit();
    _states.pop();
  }
  
  // Vuelta al estado anterior.
  if (!_states.empty())
    _states.top()->resume();
}

void GameManager::loadResources ()
{
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) 
  {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) 
    {
      typestr = i->first;    
      datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(datastr, typestr, sectionstr);	
    }
  }
}

bool GameManager::configure ()
{
  if (!_root->restoreConfig())
  {
    if (!_root->showConfigDialog())
    {
      return false;
    }
  }
  
  _renderWindow = _root->initialise(true, "Project Mini Cars");
  
  // IMPORTANTE: no llamar a este método sin haber inicializado el _renderWindow (línea anterior), de lo contrario
  // ZASCA: violación de segmento!! Al menos esto es así cuando usamos Overlays (motivo desconocido... aún)
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  
  return true;
}

GameManager* GameManager::getSingletonPtr ()
{
  return msSingleton;
}

GameManager& GameManager::getSingleton ()
{  
  assert(msSingleton);
  return *msSingleton;
}

// Las siguientes funciones miembro delegan
// el evento en el estado actual.
bool GameManager::frameStarted(const Ogre::FrameEvent& evt)
{

  _inputMgr->capture();
  return _states.top()->frameStarted(evt);
}

bool GameManager::frameEnded(const Ogre::FrameEvent& evt)
{
  return _states.top()->frameEnded(evt);
}

bool GameManager::keyPressed(const OIS::KeyEvent &e)
{
  _states.top()->keyPressed(e);
  return true;
}

bool GameManager::keyReleased(const OIS::KeyEvent &e)
{
  _states.top()->keyReleased(e);
  return true;
}

bool GameManager::mouseMoved(const OIS::MouseEvent &e)
{
  _states.top()->mouseMoved(e);
  return true;
}

bool GameManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  _states.top()->mousePressed(e, id);
  return true;
}

bool GameManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  _states.top()->mouseReleased(e, id);
  return true;
}

bool GameManager::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{
    std::cout << "GAMEMANAGER wiimoteButtonDown " <<  endl;
    _states.top()->WiimoteButtonDown(e);
    return true;
}

bool GameManager::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{
    std::cout << "GAMEMANAGER wiimoteButtonUp " <<  endl;
    _states.top()->WiimoteButtonUp(e);
    return true;
}

bool GameManager::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{
    std::cout << "GAMEMANAGER wiimoteIRMove" << endl;
    _states.top()->WiimoteIRMove(e);
    return true;
}

