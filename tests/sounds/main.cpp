#include <unistd.h>
#include "sounds.h"

int main (int argc, char * argv[])
{
  sounds *snd = sounds::getInstance();
  snd->load_xml((char*)"sounds.xml"); 
  snd->play_music("TEST_MUSIC");
  snd->play_effect("TEST_EFFECT");
  sleep (5);
  snd->halt_effects();
  snd->halt_music();
  snd->play_music("TEST_MUSIC",2);
  sleep (20);
  snd->play_effect("TEST_MUSIC",0);
  sleep(1);
  snd->play_effect("TEST_MUSIC",1);
  sleep(1);
  snd->play_effect("TEST_MUSIC",2);
  sleep(1);
  snd->play_effect("TEST_MUSIC",3);
  sleep (7);
  return 0;
}

