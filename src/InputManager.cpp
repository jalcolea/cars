#include "InputManager_.h"

#include <regex> // expresion regular, para descartar dispositivos blueetooth que no sean wiimotes de nintendo, es una cochinada :D
                 // debería meterlo en la clase Buez_util y pasar una expresión regular, de modo que busque dispositivos que cocincidan con ella

template<> InputManager_* Ogre::Singleton<InputManager_>::msSingleton = 0;

InputManager_::InputManager_ ():  _inputSystem(0),  _keyboard(0),  _mouse(0), _wiimote(0) {}

InputManager_::~InputManager_ ()
{
  if (_inputSystem) 
  {
    if (_keyboard) 
    {
      _inputSystem->destroyInputObject(_keyboard);
      _keyboard = 0;
    }

    if (_mouse) 
    {
      _inputSystem->destroyInputObject(_mouse);
      _mouse = 0;
    }

    OIS::InputManager::destroyInputSystem(_inputSystem);

    _inputSystem = 0;

    // Limpiar todos los listeners.
    _keyListeners.clear();
    _mouseListeners.clear();
    _wiimoteListeners.clear();
  }
}

void InputManager_::initialise(Ogre::RenderWindow *renderWindow)
{
  if(!_inputSystem) 
  {
    // Setup basic variables
    OIS::ParamList paramList;    
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    renderWindow->getCustomAttribute("WINDOW", &windowHnd);
    // Fill parameter list
    windowHndStr << windowHnd;
    paramList.insert(std::make_pair(std::string( "WINDOW"),windowHndStr.str()));
    
    #if defined OIS_WIN32_PLATFORM
    paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    #elif defined OIS_LINUX_PLATFORM
    paramList.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    paramList.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
    paramList.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("true")));
    paramList.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
    #endif

    // Create inputsystem
    _inputSystem = OIS::InputManager::createInputSystem(paramList);

    _keyboard = static_cast<OIS::Keyboard*>(_inputSystem->createInputObject(OIS::OISKeyboard, true));
    _keyboard->setEventCallback(this);

    _mouse = static_cast<OIS::Mouse*>(_inputSystem->createInputObject(OIS::OISMouse, true));
    _mouse->setEventCallback(this);
    
    // Get window size
    unsigned int width, height, depth;
    int left, top;
    renderWindow->getMetrics(width, height, depth, left, top);

    // Set mouse region
    this->setWindowExtents( width, height );
  }
}

// Según mi diseño inicial, voy a considera que inicializar el Wiimote implica conectarse con él.
// Se aisla la inicialización del wiimote del resto de dispositivos para no mezclar sus inicializaciones
// ya que no tienen nada que ver. El proceso de conexión del wiimote conlleva un retardo (al menos por ahora)
// por que no solo se busca un wiimote, se busca cualquier dispositivo bluetooth. Esto obviamente habría que
// mejorarlo. Primero descartando cualquier dispositivo que no fuera un wiimote y segundo tratando de hacer 
// la conexión de forma asíncrona. Lanzar la llamada de conexión y seguir con otra cosa, por ejemplo, pintando
// alguna animación indicando el proceso de conexión. Ahora mismo probablemente se bloquee el renderizado cuando
// se realice el intento de conexión.
bool InputManager_::initialiseWiimote()
{
    if (!_wiimote)
    {
        _wiimote = new wiimWrapper::Wiimote;
        Bluez_Util bluez;                // objeto que encapsula la funcionalidad para escanear dispositivos bluetooth.
        cout << "ESCANEANDO DISPOSITIVOS BLUETOOTH" << endl;
        bluez.Scanear();                // Scanea buscando dispositivos bluetooth
        if (bluez.getDevices().empty())
            return false;

        std::string cadena_patron (R"(Nintendo RVL.*)");
        std::regex patron_nombre { cadena_patron };         //Expresion regular para encontrar dispositivos con nombre "Nintento RVL ......lo que sea..... "
        for (auto &dev : bluez.getDevices())                //Nos pateamos los dispositivos encontrados
        {
            cout << "Encontrado dispositivo [" << dev.first << "] Nombre: " << dev.second << endl;
            if (regex_match(dev.second,patron_nombre))      //Si el nombre del dispositivo encontrado coincide con la expresión regular entonces
            {
                cout << "Tratando de conectar con " << dev.second << endl;
                if (_wiimote->Connect(dev.first.c_str()) >= 0)      //Si al intentar conectar con el dispositivo nos devuelve un entero no negativo entonces
                {   
                                                            // De momento el propio wrapper para manejar el wiimote activa varias propiedades del mismo al conectar
                                                            // También habría que mejorar esto y pasar como parámetro qué queremos tener activo o no: acelerómetro, infrarrojos, etc....
                                                            // O delegar en el dueño del objeto wiimote si quiere activar/desactivar propiedades haciendo llamadas a métodos
                                                            // del objeto wiimote que lo hagan. De momento no están hechas.
                                                            //Success!!!!!!! Y salimos, de momento solo manejamos un Wiimote.
                    cout << "Conexión con " << dev.second << " realizada!" << endl;
                    return true;
                }
                else
                {
                    cout << "No he podido conectarme :(" << endl;
                }
            }
            else
            {
                cout << "El dispositivo [" << dev.first << "] Nombre: " << dev.second << " no coincide con el patrón [" << cadena_patron << "]" << endl;
            }
        }    
    }
    
    return false;
    
}

void InputManager_::capture ()
{
  // Capturar y actualizar cada frame.
  if (_keyboard)
    _keyboard->capture();
    
    if (_mouse)
    _mouse->capture();
    
  if (_wiimote)
      _wiimote->Update();
}

void InputManager_::addKeyListener(OIS::KeyListener *keyListener, const std::string& instanceName)
{
  if (_keyboard) 
  {
    // Comprobar si el listener existe.
    itKeyListener = _keyListeners.find(instanceName);
    if (itKeyListener == _keyListeners.end()) 
    {
      _keyListeners[instanceName] = keyListener;
    }
    else 
    {
      // Elemento duplicado; no hacer nada.
    }
  }
}

void InputManager_::addMouseListener(OIS::MouseListener *mouseListener, const std::string& instanceName)
{
  if (_mouse) 
  {
    // Comprobar si el listener existe.
    itMouseListener = _mouseListeners.find(instanceName);
    if (itMouseListener == _mouseListeners.end()) 
    {
      _mouseListeners[instanceName] = mouseListener;
    }
    else 
    {
      // Elemento duplicado; no hacer nada.
    }
  }
}

void InputManager_::removeKeyListener(const std::string& instanceName)
{
  // Comprobar si el listener existe.
  itKeyListener = _keyListeners.find(instanceName);
  if (itKeyListener != _keyListeners.end()) 
  {
    _keyListeners.erase(itKeyListener);
  }
  else 
  {
    // No hacer nada.
  }
}

void InputManager_::removeMouseListener(const std::string& instanceName)
{
  // Comprobar si el listener existe.
  itMouseListener = _mouseListeners.find(instanceName);
  if (itMouseListener != _mouseListeners.end()) 
  {
    _mouseListeners.erase(itMouseListener);
  }
  else
  {
    // No hacer nada.
  }
}

void InputManager_::removeKeyListener(OIS::KeyListener *keyListener)
{
  itKeyListener = _keyListeners.begin();
  itKeyListenerEnd = _keyListeners.end();
  for (; itKeyListener != itKeyListenerEnd; ++itKeyListener) 
  {
    if (itKeyListener->second == keyListener) 
    {
      _keyListeners.erase(itKeyListener);
      break;
    }
  }
}

void InputManager_::removeMouseListener(OIS::MouseListener *mouseListener)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) 
  {
    if (itMouseListener->second == mouseListener) 
    {
      _mouseListeners.erase(itMouseListener);
      break;
    }
  }
}

void InputManager_::removeAllListeners()
{
  _keyListeners.clear();
  _mouseListeners.clear();
}

void InputManager_::removeAllKeyListeners()
{
  _keyListeners.clear();
}

void InputManager_::removeAllMouseListeners()
{
  _mouseListeners.clear();
}

void InputManager_::setWindowExtents(int width, int height)
{
  // Establecer la región del ratón.
  // Llamar al hacer un resize.
  const OIS::MouseState &mouseState = _mouse->getMouseState();
  mouseState.width = width;
  mouseState.height = height;
}

OIS::Keyboard* InputManager_::getKeyboard ()
{
    return _keyboard;
}

OIS::Mouse* InputManager_::getMouse ()
{
    return _mouse;
}

bool InputManager_::keyPressed(const OIS::KeyEvent &e)
{
  itKeyListener = _keyListeners.begin();
  itKeyListenerEnd = _keyListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itKeyListener != itKeyListenerEnd; ++itKeyListener) 
  {
    cout << "InputManager keypressed: " << e.key << endl;
    itKeyListener->second->keyPressed(e);
  }

  return true;
}

bool InputManager_::keyReleased(const OIS::KeyEvent &e)
{
  itKeyListener = _keyListeners.begin();
  itKeyListenerEnd = _keyListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itKeyListener != itKeyListenerEnd; ++itKeyListener) 
  {
    itKeyListener->second->keyReleased( e );
  }

  return true;
}

bool InputManager_::mouseMoved(const OIS::MouseEvent &e)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  // Delega en los MouseListener añadidos.
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) 
  {
    itMouseListener->second->mouseMoved( e );
  }

  return true;
}

bool InputManager_::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  // Delega en los MouseListener añadidos.
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) 
  {
    itMouseListener->second->mousePressed( e, id );
  }

    return true;
}

bool InputManager_::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  itMouseListener = _mouseListeners.begin();
  itMouseListenerEnd = _mouseListeners.end();
  // Delega en los MouseListener añadidos.
  for (; itMouseListener != itMouseListenerEnd; ++itMouseListener) 
  {
    itMouseListener->second->mouseReleased( e, id );
  }

  return true;
}

InputManager_* InputManager_::getSingletonPtr()
{
  return msSingleton;
}

InputManager_& InputManager_::getSingleton()
{  
  assert(msSingleton);
  return *msSingleton;
}

/*****************************************************************************/
/*                         FUNCIONALIDAD WIIMOTE                             */
/*****************************************************************************/
bool InputManager_::WiimoteButtonDown(const wiimWrapper::WiimoteEvent& arg)
{
  itWiimoteListener = _wiimoteListeners.begin();
  itWiimoteListenerEnd = _wiimoteListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itWiimoteListener != itWiimoteListenerEnd; ++itWiimoteListener) 
  {
    itWiimoteListener->second->WiimoteButtonDown(arg);
  }

  return true;
}

bool InputManager_::WiimoteButtonUp(const wiimWrapper::WiimoteEvent& arg)
{
  itWiimoteListener = _wiimoteListeners.begin();
  itWiimoteListenerEnd = _wiimoteListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itWiimoteListener != itWiimoteListenerEnd; ++itWiimoteListener) 
  {
    itWiimoteListener->second->WiimoteButtonUp(arg);
  }

  return true;

}

bool InputManager_::WiimoteIRMove(const wiimWrapper::WiimoteEvent& arg)
{
  itWiimoteListener = _wiimoteListeners.begin();
  itWiimoteListenerEnd = _wiimoteListeners.end();
  // Delega en los KeyListener añadidos.
  for (; itWiimoteListener != itWiimoteListenerEnd; ++itWiimoteListener) 
  {
    itWiimoteListener->second->WiimoteIRMove(arg);
  }

  return true;

}

void InputManager_::addWiimoteListener(wiimWrapper::WiimoteListener* wiimoteListener, const std::string& instanceName)
{
  if (_wiimote) 
  {
    // Comprobar si el listener existe.
    itWiimoteListener = _wiimoteListeners.find(instanceName);
    if (itWiimoteListener == _wiimoteListeners.end()) 
    {
      _wiimoteListeners[instanceName] = wiimoteListener;
      _wiimote->AddWiimoteListener(static_cast< shared_ptr<wiimWrapper::WiimoteListener> > (wiimoteListener));
    }
    else 
    {
      // Elemento duplicado; no hacer nada.
    }
  }

}

void InputManager_::removeWiimoteListener(const std::string& instanceName)
{
  // Comprobar si el listener existe.
  itWiimoteListener = _wiimoteListeners.find(instanceName);
  if (itWiimoteListener != _wiimoteListeners.end()) 
  {
    _wiimoteListeners.erase(itWiimoteListener);
  }
  else 
  {
    // No hacer nada.
  }
}

void InputManager_::removeWiimoteListener(wiimWrapper::WiimoteListener* wiimoteListener)
{
  itWiimoteListener = _wiimoteListeners.begin();
  itWiimoteListenerEnd = _wiimoteListeners.end();
  for (; itWiimoteListener != itWiimoteListenerEnd; ++itWiimoteListener) 
  {
    if (itWiimoteListener->second == wiimoteListener) 
    {
      _wiimoteListeners.erase(itWiimoteListener);
      break;
    }
  }

}    

void InputManager_::removeAllWiimoteListeners()
{
    _wiimoteListeners.clear();
}


