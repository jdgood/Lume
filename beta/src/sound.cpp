#include "sound.h"

using namespace irrklang;
SoundFX::SoundFX() {MusicState = 0; }
SoundFX::SoundFX(ISoundEngine* eng) : engine(eng), basePath("./sounds/") {MusicState = 0;}


void SoundFX::play(string soundF)
{
	soundFile = basePath + soundF;
   const char * fileName = soundFile.c_str();
   vec3df v;
	engine->play2D(fileName);
}

void SoundFX::play(string soundF, float x,float y, float z)
{
	soundFile = basePath + soundF;
   const char * fileName = soundFile.c_str();
	engine->play3D(fileName, vec3df(x,y,z));
}

void SoundFX::playLoop(string soundF)
{
	soundFile = basePath + soundF;
   const char * fileName = soundFile.c_str();
   if (!Music){
	   Music = engine->play2D(fileName, true, false, true);
      if (Music){
         Music->setVolume(0);
      }
      MusicState = 1;
   } else {
      NextMusic = engine->play2D(fileName, true, false, true);
      if (NextMusic){
         NextMusic->setVolume(0);
      }
      MusicState = -1;
   }
   
}

void SoundFX::playLoop(string soundF, float x,float y, float z)
{
	soundFile = basePath + soundF;
   const char * fileName = soundFile.c_str();
   irrklang::ISound* snd = engine->play3D(fileName, vec3df(x,y,z), true, false, true);
   if (snd){
     snd->setVolume(1);
     snd->setMinDistance(.2);
   }
}

void SoundFX::update(float dt)
{
   //vec3df pos = vec3df(camera.pos.px + camera.pos.x,camera.pos.py + camera.pos.y,camera.pos.pz +camera.pos.z);  
   //vec3df dir = vec3df(-camera.pos.x,-camera.pos.y,-camera.pos.z);
   vec3df pos = vec3df(character.x,character.y,character.z);  
   vec3df dir = vec3df(character.x,character.y,character.z);
   engine->setListenerPosition(pos, dir);
   if (Music){
      float vol = 0;
      if (MusicState < 0) {
         if ((vol = Music->getVolume()) > 0) {
            Music->setVolume((vol-0.08f));
         } else {
            MusicState = 0;
            Music->stop();
            Music = NULL;
            if (NextMusic) {Music = NextMusic;MusicState = 1;NextMusic = NULL;}
         }
      } else if (MusicState > 0) {
         if ((vol = Music->getVolume()) < 1) {
            Music->setVolume((vol+0.08f));
         } else {
            MusicState = 0;
         }
      }
   }
}

void SoundFX::kill()
{
	engine->drop();
}

void SoundFX::reset()
{
	engine->stopAllSounds();
}


