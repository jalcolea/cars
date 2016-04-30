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

#ifndef InputManager_H
#define InputManager_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "Wiimote.h"                                                        //WIIMOTE
#include "Bluez_Util.h"                                                     //WIIMOTE

// Gestor para los eventos de entrada (teclado y ratón).
class InputManager_ : public Ogre::Singleton<InputManager_>,
                      public OIS::KeyListener,
                      public OIS::MouseListener,
                      public wiimWrapper::WiimoteListener
{
public:
    InputManager_();
    virtual ~InputManager_();

    void initialise(Ogre::RenderWindow* renderWindow);
    void capture();
    
    bool initialiseWiimote();                                                                               //WIIMOTE


    // Gestión de listeners.
    void addKeyListener(OIS::KeyListener* keyListener, const std::string& instanceName);
    void addMouseListener(OIS::MouseListener* mouseListener, const std::string& instanceName);
    
    void addWiimoteListener(wiimWrapper::WiimoteListener* wiimoteListener, const std::string& instanceName);   //WIIMOTE

    void removeKeyListener(const std::string& instanceName);
    void removeMouseListener(const std::string& instanceName);
    void removeKeyListener(OIS::KeyListener* keyListener);
    void removeMouseListener(OIS::MouseListener* mouseListener);
    
    void removeWiimoteListener(const std::string& instanceName);                        //WIIMOTE
    void removeWiimoteListener(wiimWrapper::WiimoteListener* wiimoteListener);          //WIIMOTE

    void removeAllListeners();
    void removeAllKeyListeners();
    void removeAllMouseListeners();
    
    void removeAllWiimoteListeners();                                                   //WIIMOTE

    void setWindowExtents(int width, int height);

    OIS::Keyboard* getKeyboard();
    OIS::Mouse* getMouse();
    
    wiimWrapper::Wiimote* getWiimote();                                                 //WIIMOTE

    // Heredados de Ogre::Singleton.
    static InputManager_& getSingleton();
    static InputManager_* getSingletonPtr();

private:
    bool keyPressed(const OIS::KeyEvent& e);
    bool keyReleased(const OIS::KeyEvent& e);

    bool mouseMoved(const OIS::MouseEvent& e);
    bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);
    
    bool WiimoteButtonDown(const wiimWrapper::WiimoteEvent& arg);                                    //WIIMOTE
    bool WiimoteButtonUp(const wiimWrapper::WiimoteEvent& arg);                                      //WIIMOTE
    bool WiimoteIRMove(const wiimWrapper::WiimoteEvent& arg);                                        //WIIMOTE

    OIS::InputManager* _inputSystem;
    OIS::Keyboard* _keyboard;
    OIS::Mouse* _mouse;
    wiimWrapper::Wiimote* _wiimote;                                                     //WIIMOTE

    std::map<std::string, OIS::KeyListener*> _keyListeners;
    std::map<std::string, OIS::MouseListener*> _mouseListeners;
    std::map<std::string, wiimWrapper::WiimoteListener*> _wiimoteListeners;             //WIIMOTE
    std::map<std::string, OIS::KeyListener*>::iterator itKeyListener;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListener;
    std::map<std::string, OIS::KeyListener*>::iterator itKeyListenerEnd;
    std::map<std::string, OIS::MouseListener*>::iterator itMouseListenerEnd;
    std::map<std::string, wiimWrapper::WiimoteListener*>::iterator itWiimoteListener;   //WIIMOTE
    std::map<std::string, wiimWrapper::WiimoteListener*>::iterator itWiimoteListenerEnd;//WIIMOTE
};
#endif
