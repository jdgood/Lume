#if ! defined _SHADOW_H
#define _SHADOW_H 1

#include "util.h"
#include "obj.h"

#define M_INFINITY 100

class shadow {
  public:
     pnt3d light;
     shadow(float x_in,float y_in,float z_in){light = pnt3d(x_in,y_in,z_in);};
     void draw(obj* o);
     void make(obj* o, int index);
};

void shadowPlane();

#endif
