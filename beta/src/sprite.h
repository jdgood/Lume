#ifndef SPRITE
#define SPRITE 1

#include <stdio.h>
#include "util.h"
#include <string>
#include <vector>
#include "tex.h"

using namespace std;

#define FRAMESEC 50
#define PLAY_ONCE 0
#define PLAY_LOOP 1

class Sprite {

   public:
      vector<GLuint> indexes;
      int id;//current texure id
      int frame;//current frame
      float time;//time since las frame
      int spf;//seconds per frame
      bool isLooping;//loop if true
      bool finished;//play if false

      Sprite();
	   void load(const char* tex, int f);
	   void load(const char* tex, int f, int newspf);
	   void preLoad(const char* tex, int f);
	   void loadjpg(const char* tex, int f);
      void load(char *tex);
      void load(GLuint tex);
      void play();
      void play(bool loop);
      void play(int secondspf);
      void end();
      void update(float dt);

};
#endif
