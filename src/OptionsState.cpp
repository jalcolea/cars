#include "OptionsState.h"
#include "PauseState.h"
#include "IntroState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "records.h"
#include "SceneNodeConfig.h"

template<> OptionsState* Ogre::Singleton<OptionsState>::msSingleton = 0;

using namespace std;
using namespace Ogre;

void OptionsState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _viewport = _root->getAutoCreatedWindow()->getViewport(0);
  _viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 1.0));
  
  createScene();
  _exitGame = false;
}

void OptionsState::exit ()
{
 //_sceneMgr->clearScene();
  _root->getAutoCreatedWindow()->removeAllViewports();

  destroyMyGui();
}

void OptionsState::pause()
{
}

void OptionsState::resume()
{
}

bool OptionsState::frameStarted(const Ogre::FrameEvent& evt)
{
  return true;
}

bool OptionsState::frameEnded(const Ogre::FrameEvent& evt)
{
  return true;
}


bool OptionsState::keyPressed(const OIS::KeyEvent &e)
{
  if (e.key == OIS::KC_S) {
    sounds::getInstance()->play_effect("push");
    MyGUI::LayoutManager::getInstance().unloadLayout(layout);
    pushState(PlayState::getSingletonPtr());
  }
  else
{
  sounds::getInstance()->play_effect("push");
  popState();
}
  
  return true;
}

bool OptionsState::keyReleased(const OIS::KeyEvent &e)
{
  return true;
}

bool OptionsState::mouseMoved(const OIS::MouseEvent &e)
{
  return true;
}

bool OptionsState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  int x = e.state.X.abs;
  int y = e.state.Y.abs;
  if (btn_back->_checkPoint(x,y))
  {
    sounds::getInstance()->play_effect("push");
    popState();
  }
  /*else if (btn_mouse->_checkPoint(x,y)) 
  {
    sounds::getInstance()->play_effect("push");
    text->setCaption("MOUSE Modo de control recomendado");
  }
  else if (btn_wiimote->_checkPoint(x,y)) 
  {
    sounds::getInstance()->play_effect("push");
    text->setCaption("WIIMOTE Disponible solo en modo desarrollador");
  }
*/
  else text->setCaption("");
  return true;
}

bool OptionsState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  return true;
}

OptionsState* OptionsState::getSingletonPtr()
{
    return msSingleton;
}

OptionsState& OptionsState::getSingleton()
{ 
  assert(msSingleton);
  return *msSingleton;
}

OptionsState::~OptionsState()
{
}


void OptionsState::createScene()
{

    // Configuramos la cámara 
    double width = _viewport->getActualWidth();
    double height = _viewport->getActualHeight();
    _camera->setAspectRatio(width / height);
    // La he puesto aquí pero esto hay que probar donde quede mejor
    _camera->setPosition(0,4,9);
    // Le digo que mira al centro exacto, el coche lo pongo en el centro así que mirará hacia él
    _camera->lookAt(0,0,0);
    _camera->setNearClipDistance(0.1); // Creo que esto era la configuración del frustrum de la cámara.
    _camera->setFarClipDistance(100);


    // Añado luz, si no se ve 'to' oscuro :D
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

  
  // Desactivo MYGUI pq si no, no se ve. Hay que poner el layout por detrás del coche y yo no sé (ahora claro)  
  createMyGui();                                          
  //_sceneMgr->createStaticGeometry("CAR");
  ent_car =  _sceneMgr->createEntity("ent_car","car-groupc-1.mesh");
  ent_car->setCastShadows(true);                            // para que proyecte sombras
  node_car = _sceneMgr->createSceneNode("node_car");
  node_car->attachObject(ent_car);
  _sceneMgr->getRootSceneNode()->addChild(node_car);
  //node_car->setPosition(Ogre::Vector3(3.25,-5.0,18));
  
  // Lo he puesto en el centro para comprobar que efectivamente estaba ahí
  node_car->setPosition(Ogre::Vector3(0,0,0));
  // Lo giro un poco para que se vea molón :D             
  node_car->yaw(Ogre::Degree(45));
  
  
  /* Aparte del tema de la cámara, MyGui pinta su layout por encima de todo lo demás, 
   * es decir, lo pinta lo último de todo por lo que taparía cualquier cosa pintada,
   * como el coche en este caso. No sé si hay alguna opción en MyGui
   * para cambiar esto. Se tendría que poder. Por ejemplo, cuando ponemos un fondo
   * (como en la imagen que se ve en la intro del Pacman) se le tiene que indicar a 
   * Ogre que lo pinte lo primero de todo:
   * _rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
   * _rect es un rectángulo con una textura (el fondo) aplicada y con esa instrucción
   * le indicamos a ogre que es el fondo, o sea, que lo pinta antes que nada más.
   * Si no se puede pues tendrás que hacer un layout con una parte transparente para que
   * se vean los coches a seleccionar.
   * Por otra parte, la idea que yo tenía es la siguiente: cogemos un plano que haga de
   * suelo para los coches a seleccionar. Por ejemplo podemos hacer un círculo plano en 
   * Blender, le aplicamos una textura de asfalto y lo ponemos como suelo. El nodo de este
   * plano lo hacemos padre de los coches. Ahora cargamos todos los coches (entidades y nodos)
   * y hacemos sus nodos hijos del nodo suelo. Los colocamos haciendo un círculo y ahora
   * cuando le demos a las flechas solo tenemos que rotar el suelo los grados que sean de modo
   * que el coche seleccionado quede en la posición (0,0,-1) con respecto a ese plano.
   * Si por ejemplo tenemos 6 coches a seleccionar, pues colocamos uno cada 60 grados. Cada
   * vez que le demos a la flecha rotamos 60 grados el plano y así vamos cambiando de coche.
   * Una vez funcionase esto, pues ya le podemos dar algún toque vistoso como por ejemplo que 
   * el coche seleccionado se ponga a rotar despacio para que se vea desde todos los ángulos
   * y que se levantara un poco y también podríamos escalarlo haciéndolo un poco más grande
   * dando a entender que el coche está seleccionado.
   */

}

void OptionsState::destroyMyGui()
{
  MyGUI::LayoutManager::getInstance().unloadLayout(layout);
}

string OptionsState::get_high_score()
{
  string recordname;
  int recordpoints;
  char msg [128];
  records::getInstance()->getBest (recordname,recordpoints);
  sprintf (msg,"%s %d",recordname.c_str(),recordpoints);
  return string(msg);
}

void OptionsState::createMyGui()
{
  layout = MyGUI::LayoutManager::getInstance().loadLayout(LAYOUT_OPTIONS);
  btn_back = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_back");
  //btn_mouse = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_mouse");
  //btn_wiimote = MyGUI::Gui::getInstance().findWidget<MyGUI::Button>("btn_wiimote");
  text = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("text");
  edt_high = MyGUI::Gui::getInstance().findWidget<MyGUI::EditBox>("edt_high");
  //edt_high->setCaption(get_high_score());

}

bool OptionsState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool OptionsState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e)
{return true;}
bool OptionsState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e)
{return true;}
