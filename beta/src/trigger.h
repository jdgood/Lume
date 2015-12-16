#if ! defined _TRIGGER_H
#define _TRIGGER_H 1

#include "obj.h"
#include "util.h"
#include "sdata.h"
#include "player.h"


extern SData world;

class trigger : public obj {
   public:
      bool hit;
      bool updateDetail;
      bool downloadbar;
      char name[50];
      trigger(float x,float y,float z,float dx,float dy,float dz, char* name);
      void update(float dt);
      //bool hitTest(float ix,float iy,float iz);
      bool hitCamera(float ix,float iy,float iz);
      bool hitCollision(obj object);
};

#endif
