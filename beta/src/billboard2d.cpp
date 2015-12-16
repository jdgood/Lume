#include "billboard2d.h"

billboard2d::billboard2d(float sx, float sy, float sz, int tex, float _width, float _height, int life, pnt3d dir){//billboarding floater
   classId = FLOATER2D_ID;// BB2D_ID;
   n = dir;
   x = sx;
   y = sy;
   z = sz;
   offset = 0;
   width = _width;
   height = _height;
   ref = NULL;
   extex = NULL;
   parent = NULL;
   texture.id = tex;
   lifetime = life;
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
}

void billboard2d::update(float dt){
   if (lifetime > 0){
       lifetime-=dt;
       if (lifetime < 500) alpha-= dt/50;
       if (lifetime < 1) kill();
   }
   //n = pnt3d(camera.pos.x/camera.pos.mag(),camera.pos.y/camera.pos.mag(),camera.pos.z/camera.pos.mag());//This is FAILLLLLL
}
