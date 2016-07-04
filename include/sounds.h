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
    int play_music(string id, int times=-1, int initVolume = MIX_MAX_VOLUME);
    void pause_music();
    int paused_music();
    void resume_music();
    int halt_music();
    int halt_effect(int ch = -1);
    void print();
    void set_channel (int ch);
    void unset_channel ();
    inline int isMixPlaying(int canal){ return Mix_Playing(canal); };
    inline Mix_Chunk* getMixChunk(string nombreEfecto){ return map_effect[nombreEfecto]; };
    inline void setVolume(string nombreEfecto, int volumen){map_effect[nombreEfecto]->volume = volumen;};
    inline void setMusicVolume(int volumen){ Mix_VolumeMusic(volumen); };
    inline int getMusicVolume(){ return Mix_VolumeMusic(-1); }; // Al pasarle -1 a Mix_VolumeMusic indicamos que no queremos cambiar el volumen, solo recuperar valor actual.
    inline int maxVolume(){ return MIX_MAX_VOLUME; };

};

#endif
