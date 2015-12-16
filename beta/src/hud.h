#if ! defined _HUD_H
#define _HUD_H 1

#include "util.h"
#include "tex.h"
#include "player.h"
#include <dirent.h>
#include "script.h"
#include "sdata.h"
#include "level.h"
#include "billboard2d.h"
#include "particle.h"

#define HUD_PIXEL_RATE 5

extern Player character;
extern bool bPause;
extern SData world;
extern Level* currentLevel;
extern bool canLoad;

const int RESUME = 1;
const int SAVE_GAME = 2;
const int NEW_GAME = 3;
const int LOAD_GAME = 4;
const int EXIT = 5;

typedef struct lvlup{
  char str[20];
  float x;
  float y;
  float color[4];
  lvlup(const char* n,float x_in, float y_in):x(x_in),y(y_in){
      strcpy(str,n);
      color[0]=color[1]=color[2]=color[3]= 1.0;};
  lvlup(const char* n,float x_in, float y_in,float r, float g, float b):x(x_in),y(y_in){
      strcpy(str,n);
      color[0]=r;
      color[1]=g;
      color[2]=b;
      color[3]= 1.0;};
} lvlup;

class HUD{
  public:
     float energyPixels;
     float expPixels;
     int barstex;
     int bluebar;
     int orangebar;
     Sprite menutex;
     Sprite loadtex;
     Sprite sprinttex;
     Sprite introtex;
     Sprite outrotex;
     Sprite download;
     script Insight;
     int curlvl;
     int totalEnergy;
     bool end_flag;
     HUD();
     void loadStart();
     void loadMenu();
     void draw();
     void drawLoadScreen();
     void drawIntro();
     void drawOutro();
     int drawMainMenu(int x, int y);
     int loadGameMenu();
     void update(float dt);
     void add(const char* n,float nx, float ny);
     void add(const char* n,float nx, float ny, float r, float g, float b);
  private:
     std::vector<lvlup> lvlups;
};

#endif
