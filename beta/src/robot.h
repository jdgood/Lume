#if ! defined _ROBOT_H
#define _ROBOT_H 1

#include "cam.h"
#include "obj.h"
#include "model.h"
#include "shader.h"
#include "player.h"
#include "sound.h"
#include "sdata.h"

//#define GRAVACC .03 //gravital acceleration
//#define GRAVTERM 0.9 //terminal velocity
#define MAXJUMP .15 //jump velocity
#define MAXSPEED .1 //movement speed

extern Shader* shader;
extern FBO* fbo;
extern bool editmode;
extern float grav;
extern Player character;
extern SoundFX sound;
extern SData world;

class Robot : public model{
      public:
      // Movement
      pnt3d vel;	   
      pnt3d dir;
      float by;
      float maxspeed;
      int laststate;
      int state; // 0-still 1-walking 2-jump 3-build (the idle animation)
      float walkacc;
      vector<pnt3d> nodes;
      unsigned int curDest;
      //int robotId;
      bool chase;
		bool oldchase;
		bool returning;
      int rando;
      int frozen;
      std::vector<obj*> objs;

      //loading stages
      bool gameStarted;
      int stage;
      int loadingStage;

      //fullLoad
      bool fullLoad;
      
      //path test
      bool pathDirection;
      bool randomizer;
      
      //float xrot;
      //float yrot;
      Robot();
      Robot(float x, float y, float z);
      Robot(pnt3d init);
      float distance();
      void addNode(pnt3d);
      void click(float cx, float cy, float cz);
      void draw();
      float calcVelocityMagn(pnt3d vel);
      void cleanup();
      int checkMove();
      void update(float dt);
      float distance(obj* o);
      void simplePath(int direction, float start, float end); 
      void update();

		int angle;  
};
#endif 
