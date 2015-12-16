#if ! defined _PLAYER_H
#define _PLAYER_H 1

#include "FBO.h"
#include "cam.h"
#include "obj.h"
#include "model.h"
#include "shader.h"
#include "sdata.h"

//#define GRAVACC .03 //gravital acceleration
//#define GRAVTERM 0.9 //terminal velocity
#define MAXJUMP .15 //jump velocity
#define MAXSPEED .1 //movement speed

extern cam camera;
extern FBO* fbo;
extern Shader* shader;
extern Shader* bloom;
extern SData world;
extern bool editmode;
extern float grav;
extern bool jumpActive;
extern bool isBuilding;
extern bool isRemoving;

class Player : public model{
      public:
      // Energy 
      int curenergy;
      int totalenergy;
      float exp;
      int expNeeded;
      int level;
      int buildlvl;
      // Movement
      pnt3d vel;	   
      pnt3d dir;
      float maxspeed;
      float maxjump;
      bool isJumping;
      bool isSprinting;
      bool canSprint;
      float sprintEnergy;
      int laststate;
      int state; // 0-still 1-walking 2-jump 3-build (the idle animation)
      float lastY;
      float walkacc;
      pnt3d checkpoint;
      int frameCount;

      //object control
      obj * target;

      //loading stages
      bool gameStarted;
      int stage;
      int loadingStage;

      //fullLoad
      bool fullLoad;
      
      //void (*lvlFunc)();    

      //float xrot;
      //float yrot;
      Player();
      Player(float x, float y, float z);
      Player(float x, float y, float z, int curenergy, int totalenergy, float maxspeed, float maxjump);
      void draw();
      float calcVelocityMagn(pnt3d vel);
      void cleanup();
      //void setPos(float xp, float yp, float zp);
      void setLevel(int l);
      void addExp(float e);
      void addEnergy(int e);
      void setEnergy(int en);
      void setTotalEnergy(int en);
      void setCheckpoint(float xcheck, float ycheck, float zcheck);
      int checkMove(std::vector<obj*> objs);
      int checkTarget(std::vector<obj*> objs);
      void update(float dt, std::vector<obj*> objs);
      void respawn();
      void control(obj *o);
      void releaseControl();
      float distance(obj* o);
      void setVel(float dx, float dz);  

      //void addListener(debug::void (*lvlF)());
};
#endif 
