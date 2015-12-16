#if ! defined _SCRIPT_H
#define _SCRIPT_H 1

#include "util.h"
#include "tex.h"
#include "player.h"
#include "sound.h"

#define SCRIPT_LOC .75//.875

extern SoundFX sound;

typedef struct message{
  char prompt[100];
  float hold;
  message(const char* n):hold(200){ strcpy(prompt,n);};
  message(const char* n,float h):hold(h){ strcpy(prompt,n);};
} message;

class script {
  public:
     int symbol;
     unsigned int curMessage;     
      //float holdtime;
     float y;
     bool visible;
     bool enabled;
     script();
     void clear();
     void resize();
     void update(float dt);
     void show();
     void showFirst(const char * text);
     void showFirst(const char * text, float hold);
     void show(const char * text);
     void show(const char * text, float hold);
     void draw();
     void drawBack();
     std::vector<message> messages;
};

#endif 
