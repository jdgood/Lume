#if ! defined _GOBJ_H
#define _GOBJ_H 1

#include "util.h"
#include "obj.h"
#include "player.h"
#include "sdata.h"

extern Player character;
extern SData world;

#define GROWSPEED 0.02
#define GOBJ_ID 3
class GrowingObj : public obj{
  public:
    float gx;//goal x,y,z to be obtained by growing
    float gy;
    float gz;
    pnt3d dir;
    bool growing;
    GrowingObj(float in_x,float in_y,float in_z):obj(in_x, in_y, in_z),gx(0),gy(0),gz(0){classId = GOBJ_ID;colored = 0;litColor = -1;visible = true;growing=true;};
    void update(float dt);//overwrite update
    void click(float cx, float cy, float cz);
    void ungrow(float dt);
    void resize();
    pnt3d getDir(){return dir;};
  //private:
};

GrowingObj* newGrowingCube(float x,float y,float z,float dx,float dy,float dz);

#endif
