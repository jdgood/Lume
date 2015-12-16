#if ! defined _SOUND_H
#define _SOUND_H 1

#include <stdio.h>
#include <irrKlang.h>
#include <string>
#include "obj.h"
#include "cam.h"
#include "player.h"

extern cam camera;
extern Player character;

using namespace std;

//using namespace irrklang;


//#pragma comment(lib, "irrKlang.lib") 

class SoundFX {

   public:

      //string fileName;

      irrklang::ISoundEngine* engine;
      string soundFile;
      string basePath;

      irrklang::ISound* Music;
      irrklang::ISound* NextMusic;
      int MusicState;//-1 fade out (load next song if present), 1 fade in, 0 nothing

      SoundFX();
      //Sound(irrklang::ISoundEngine* eng);
      SoundFX(irrklang::ISoundEngine* eng);

      void play(string soundF);
      void play(string soundF, float x,float y, float z);
      void playLoop(string soundF);
      void playLoop(string soundF, float x,float y, float z);
      void update(float dt);
      void kill();
      void reset();
};

#endif
