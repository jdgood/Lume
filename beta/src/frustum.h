#if ! defined _FRUSTUM_H
#define _FRUSTUM_H 1

#include "util.h"
#include "tex.h"
#include "sprite.h"
#include "obj.h"
#include "cam.h"

extern cam camera;

struct Plane{
  float a, b, c, d;
};

class frustum {
  public:
     float m[16];
     Plane planes[6];
     virtual bool PointInFrustum(pnt3d p);
     virtual bool radiusInFrustum(pnt3d p, float radius);
     virtual bool PointInFrustumV(pnt3d p);
     virtual bool radiusInFrustumV(pnt3d p, float radius);
     virtual bool needsCull(obj* object);
     virtual bool shadowNeedsCull(obj* object);
     virtual bool needsCull(obj2d* object);
     virtual void updateMatrix(cam camera);
     pnt3d canonic(float x, float y, float z);
     virtual void printm();
};

#endif
