
#include "ControlsState.h"
#include "RecordsState.h"
#include "PauseState.h"
#include "IntroState.h"
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

#define CAMSPEED 20
#define CAMROTATESPEED 0.1
template <> PlayState *Ogre::Singleton<PlayState>::msSingleton = 0;

using namespace std;
using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

void PlayState::enter() {
  _root = Ogre::Root::getSingletonPtr();
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->getCamera("IntroCamera");
  _cameraNode = _sceneMgr->createSceneNode("nodeCamera");
  _camera->setPosition(Vector3::ZERO);
  _cameraNode->attachObject(_camera);
  _sceneMgr->getRootSceneNode()->addChild(_cameraNode);
  _exitGame = false;
  paused = false;
  
  _deltaT = 0;
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
  moveCamera();
  checkCollisions();

  if (keysArePressed[OIS::KC_ESCAPE]) return false;
  return true;
}

bool PlayState::frameEnded(const Ogre::FrameEvent &evt) {
  if (keysArePressed[OIS::KC_ESCAPE]) return false;
  else return true;
}

bool PlayState::keyPressed(const OIS::KeyEvent &e) {

  std::cout << "presionando " << std::endl;
  keysArePressed.erase(e.key);
  keysArePressed[e.key] = true;
  if (e.key == OIS::KC_P) {
    // pushState(PauseState::getSingletonPtr());
  } else if (e.key == OIS::KC_G) {
    //    game_over();
  } else if (e.key == OIS::KC_W) {
    //    win();
  }
 
  return true;
}
/**
 * Move the camera around the world if arrows are pressed
 */
void PlayState::moveCamera() {
}

bool PlayState::keyReleased(const OIS::KeyEvent &e) { return true;
}


bool PlayState::mouseMoved(const OIS::MouseEvent &e) { return true;
}

bool PlayState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {
  return true;
}

bool PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {
  return true;
}

PlayState *PlayState::getSingletonPtr() { return msSingleton; }

PlayState &PlayState::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

PlayState::~PlayState() {}

void PlayState::createScene() {

  createLight();
  createMyGui();
}

void PlayState::createMyGui() {}

void PlayState::destroyMyGui() {}

void PlayState::createLight()
{
}

void PlayState::win() {}

void PlayState::game_over() {}

void PlayState::set_lives(int lives) { this->lives = lives; }

int PlayState::get_lives() { return lives; }

void PlayState::set_score(int score) { this->score = score; }

int PlayState::get_score() { return score; }

void PlayState::handleCollision(btCollisionObject *body0,
                                btCollisionObject *body1) {}

bool PlayState::WiimoteButtonDown(const wiimWrapper::WiimoteEvent &e) {
  return true;
}
bool PlayState::WiimoteButtonUp(const wiimWrapper::WiimoteEvent &e) {
  return true;
}
bool PlayState::WiimoteIRMove(const wiimWrapper::WiimoteEvent &e) {
  return true;
}

void PlayState::checkCollisions() {
}
