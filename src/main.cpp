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

#define UNUSED_VARIABLE(x) (void)x

#include "GameManager.h"
#include "IntroState.h"
#include "PlayState.h"
#include "OptionsState.h"
#include "PauseState.h"
#include "WinState.h"
#include "LooseState.h"
#include "MenuState.h"
#include "ControlsState.h"
#include "RecordsState.h"
#include "CreditsState.h"
#include "sounds.h"
#include "records.h"
#include "testStateVehicRayCast.h"
#include "carSelectorState.h"
#include "pathDrawerState.h"
#include "actualOptions.h"

#include <iostream>

using namespace std;

int main () {
  
  srand(time(NULL));

  GameManager* game = new GameManager();
  IntroState* introState = new IntroState();
  PlayState* playState = new PlayState();
  OptionsState* optionsState = new OptionsState();
  MenuState* menuState = new MenuState();
  ControlsState* controlsState = new ControlsState();
  PauseState* pauseState = new PauseState();
  WinState* winState = new WinState();
  LooseState* looseState = new LooseState();
  RecordsState * recordState = new RecordsState();
  CreditsState * creditState = new CreditsState();
  testStateVehicRayCast* test = new testStateVehicRayCast();
  carSelectorState* selector = new carSelectorState();
  pathDrawerState* pathDrawer = new pathDrawerState();
  SceneNodeConfig* snc = new SceneNodeConfig();
  actualOptions* actOpt = new actualOptions();
  records * r = records::getInstance();
  sounds * s = sounds::getInstance();

  UNUSED_VARIABLE(introState);
  UNUSED_VARIABLE(playState);
  UNUSED_VARIABLE(pauseState);
  UNUSED_VARIABLE(menuState);
  UNUSED_VARIABLE(controlsState);
  UNUSED_VARIABLE(winState);
  UNUSED_VARIABLE(looseState);

  s->load_xml((char*)"sounds.xml");
  r->loadFile((char*)"records.txt");
  try
    {
      // Inicializa el juego y transición al primer estado.
      game->start(IntroState::getSingletonPtr());
    }
  catch (Ogre::Exception& e)
    {
      std::cerr << "Excepción detectada: " << e.getFullDescription();
    }
  
  delete game;
  
  return 0;
}
