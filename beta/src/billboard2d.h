#if ! defined _BB2D_H
#define _BB2D_H 1

#include "obj.h"
#include "cam.h"
extern cam camera;

class billboard2d : public obj2d {
	public:
      billboard2d(){};
      billboard2d(float sx, float sy, float sz, int tex, float _width, float _height, int life, pnt3d dir);
      void update(float dt);
};

#endif
