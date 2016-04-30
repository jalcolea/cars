#ifndef __SOUNDS__
#define __SOUNDS__

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <vector>
#include <mxml.h>
#include <map>

using namespace std;

typedef enum sound_type {MUSIC,EFFECT} sound_type;
typedef std::map <std::string, Mix_Music*>::iterator it_map_music;
typedef std::map <std::string, Mix_Chunk*>::iterator it_map_effect;


class sounds {
  private:
    std::map <string,Mix_Music *> map_music;
    std::map <string,Mix_Chunk *> map_effect;
    static sounds * singleton;
    int channel;
    void new_sound(mxml_node_t * node, int type);
    string base_path;
    bool init ();
    void clean_up ();
    sounds();
  public:
    static sounds * getInstance();
    ~sounds(); 

    bool load_xml (char * file=(char*)"sounds.xml");

    int halt_effects(int ch=-1);
    int play_effect(string id, int ch=-1);
    int play_effect_loop(string id, int ch=-1);

    int playing_music();
    int play_music(string id, int times=-1);
    void pause_music();
    int paused_music();
    void resume_music();
    int halt_music();
    int halt_effect(int ch = -1);
    void print();
    void set_channel (int ch);
    void unset_channel ();

};

#endif
