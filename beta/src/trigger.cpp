#include "trigger.h"

trigger::trigger(float nx,float ny,float nz,float dx,float dy,float dz, char* n) {
   x = nx;
   y = ny;
   z = nz;
   visible = false;
   hidden = true;
   hit = false;
   downloadbar = false;
   classId = TRIGGER_ID;
   addCube(this,dx,dy,dz);
   updateDetail = false;
   strcpy(name, n);
}

void trigger::update(float dt){

}

/*bool trigger::hitTest(float ix,float iy,float iz){
   if (obj::hitTest(ix, iy, iz)){
     hit = true;
     return false;
   }
   else{
     return false;
   }
}*/

bool trigger::hitCamera(float ix,float iy,float iz){
   return false;
}

bool trigger::hitCollision(obj object){
   if (obj::hitCollision(object)){
     
      if (!hit){ //only generate first time trigger hit
         /*//download insight objectives
         world.add(new obj2d(x,y-1.0,z,&(camera.pos),100, true));
         world.add(new obj2d(x,y-0.5,z,&(camera.pos),100, true));
         world.add(new obj2d(x,y,z,&(camera.pos),100, true));
         world.add(new obj2d(x,y+0.5,z,&(camera.pos),100, true));
         world.add(new obj2d(x,y+1.0,z,&(camera.pos),100, true));*/

         //download insight objectives
         world.add(new obj2d(x,y + ymin + 0.01,z,&(camera.pos),280, true));//280 represents lifetime of ring
         world.add(new obj2d(x,y + ymin + 0.01,z,&(camera.pos),260, true));//at 200 they begin to raise off ground
         world.add(new obj2d(x,y + ymin + 0.01,z,&(camera.pos),240, true));//the distance between relies on the time in between
         world.add(new obj2d(x,y + ymin + 0.01,z,&(camera.pos),220, true));//the order must be decending otherwise it will not draw right
         world.add(new obj2d(x,y + ymin + 0.01,z,&(camera.pos),200, true));
         hit = true;
         updateDetail = true;
      }
      return false;
   }
   else{
     return false;
   }
}

