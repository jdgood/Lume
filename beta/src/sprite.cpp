#include "sprite.h"
#include "util.h"
#include <string>
#include <vector>
#include "tex.h"

using namespace std;

Sprite::Sprite() {finished = true; isLooping = true; id=0; time = 0; frame = 1;spf = FRAMESEC;}

/**
 * This loads the textures under the starting name of tex
 * and has the known amount of f frames in the animation.  
 * The textures are looped through if an explicit texture is not
 * set (o->texture == -1).
 */
void Sprite::load(const char *tex, int f)
{
   if (f < 1) return;
   char word[64];
	GLuint tempInt;
   bool doesExist = false;
	for(int i = 0; i < f; i++){
      if (!doesExist) {
         sprintf(word, "textures/%s/%s%03d.jpg", tex, tex, i);
		   tempInt = LoadTexture(word, &doesExist);
	      indexes.push_back(tempInt);
      }
      else {
	      indexes.push_back(tempInt + i);
      }
  	}
   id = indexes[1];
}

void Sprite::load(const char *tex, int f, int newspf)
{
   if (f < 1) return;
   char word[64];
	GLuint tempInt;
	for(int i = 0; i < f; i++){
      sprintf(word, "textures/%s/%s%03d.jpg", tex, tex, i);
		tempInt = LoadTexture(word);
	   indexes.push_back(tempInt);
  	}
   spf = newspf;
   id = indexes[1];
}

void Sprite::preLoad(const char *tex, int f)
{
   if (f < 1) return;
   char word[64];
	GLuint tempInt;
   sprintf(word, "textures/%s/%s%03d.jpg", tex, tex, f);
	tempInt = LoadTexture(word);
	indexes.push_back(tempInt);
   id = indexes[1];

}

void Sprite::load(char *tex)
{
	id = LoadTexture(tex);
	indexes.push_back(id);
}

void Sprite::load(GLuint tex)
{
	id = tex;
	indexes.push_back(id);
}

void Sprite::play()
{
   frame = 1;
   finished = false;
}

void Sprite::play(bool loop)
{
   isLooping = loop;
}

void Sprite::play(int secondspf)
{
   spf = secondspf;
   //frame = 1;
   isLooping = false;
   finished = false;
}

void Sprite::end()
{
   isLooping = false;
   frame = 0;
}

void Sprite::update(float dt)
{
   if (indexes.size() <= 1){ return;}  
   if ((time >= spf)) {
      if (isLooping || !finished) {
         frame++;
      }
      else
         frame = 0;
      if (frame >= (int)indexes.size()) {
           if (isLooping)
              frame = 1;
           else
              frame = 0;
           finished = true;
      }
      id = indexes[frame];
      time = 0;
   }
   time += (float)dt*TIMERINT;
}
