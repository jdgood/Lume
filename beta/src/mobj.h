#if ! defined _MOBJ_H
#define _MOBJ_H 1

#include "util.h"
#include "obj.h"
#include "player.h"
#include "sdata.h"
#include "tex.h"

extern Player character;
extern SData world;

#define WAIT_TIME 500
#define BELT_SPEED 4

class MovingObj : public obj{
   vector<pnt3d> nodes;
   vector<float> speeds;
   pnt3d vel;
   unsigned int curDest;
   bool controllable;
   bool controlled;
   bool active;
   int timeWaiting;
   int visibleTimer;
   int oldTimer;
   std::vector<obj*> objs;
   
   public:
   void setActive(){active = true;}
   void addNode(pnt3d, float);
   void update(float dt);
   void click(float cx, float cy, float cz);
   int checkTarget();
   
   MovingObj(pnt3d init, float dx, float dy, float dz, char* tname, bool _active, float sc, bool cont);
};
#endif
