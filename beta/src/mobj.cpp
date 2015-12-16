#include "mobj.h"

MovingObj::MovingObj(pnt3d init, float dx, float dy, float dz, char* tname, bool _active, float sc, bool cont){
   curDest = 0;
   active = _active;
   controlled = false;
   controllable = cont;
   timeWaiting = 0;
   nodes.push_back(init);
   speeds.push_back(sc);
   visibleTimer = WAIT_TIME/5;
   oldTimer = 0;
   
   x = init.x; y = init.y; z = init.z;
   
   classId = MOVINGOBJ_ID;
   
   texture.load(tname);

   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(.8,.8,.8));
   }
   addCube(this,dx,dy,dz);
}

void MovingObj::update(float dt){
   if (!active) 
      return;

   if (pnt3d((nodes[0].x - nodes[2].x) / 2, (nodes[0].y - nodes[2].y) / 2,
           (nodes[0].z - nodes[2].z) / 2).distBetween(pnt3d(character.x, character.y, 
               character.z)) > 180 && !strcmp(curStage, "skyline")) {
      x = nodes[0].x;
      y = nodes[0].y;
      z = nodes[0].z;
      curDest = 0;
      return;
   }
   
   objs = world.getObjs(x,y,z);

   if (!controlled && !timeWaiting) {
       if (nodes.size() >= 2) {
         if (nodes[0].x == nodes[1].x &&
             nodes[0].y == nodes[1].y &&
             nodes[0].z == nodes[1].z) {
             if (zmax > ymax && zmax > xmax)
               vel.z = BELT_SPEED;
             else if (xmax > ymax && xmax > zmax)
               vel.x = BELT_SPEED;
             else if (ymax > xmax && ymax > zmax)
               vel.y = BELT_SPEED;
         }
         else {
            if(.1 > abs(x - nodes[curDest].x) && 1 > abs(y - nodes[curDest].y) && .1 > abs(z - nodes[curDest].z)){
               curDest++;
               if(curDest == nodes.size()){
                  curDest = 0;
               }
               //calculate velocity(x,y,z and nodes[curDest]x,y,z) and normalize
               vel.x = nodes[curDest].x - x;
               vel.y = nodes[curDest].y - y;
               vel.z = nodes[curDest].z - z;
               vel = vel.normalize(vel); 
            }
                 
            x += vel.x * dt * speeds[curDest];
            if (checkTarget()) {
               x -= vel.x * dt * speeds[curDest];
               nodes[curDest].x = x;
               nodes[curDest].y = y;
               nodes[curDest].z = z;
               curDest++;
            }
            
            y += vel.y * dt * speeds[curDest];
            if (checkTarget()) {
               y -= vel.y * dt * speeds[curDest];
               nodes[curDest].x = x;
               nodes[curDest].y = y;
               nodes[curDest].z = z;
               curDest++;
            }
               
            z += vel.z * dt * speeds[curDest];
            if (checkTarget()) {
               z -= vel.z * dt * speeds[curDest];
               nodes[curDest].x = x;
               nodes[curDest].y = y;
               nodes[curDest].z = z;
               curDest++;
            }
            
            if(curDest == nodes.size()){
               curDest = 0;
            }
         }

         character.y -= 1; // my bad (when standing on object you do not collide with it)
         // to fix this we add an offset of .05 to check if he is standing on it then correct the offset later
         if (hitCollision(character)) {
            character.x += vel.x * dt * speeds[curDest];
            character.y += vel.y * dt * speeds[curDest];
            character.z += vel.z * dt * speeds[curDest];
         } 
         character.y += 1;
         
         //check if mobj runs into character (side)
         /*if (vel.x > 0)
            while (hitCollision(character)) character.x+= 0.01;
         else if (vel.x < 0)
            while (hitCollision(character)) character.x-= 0.01;
         if (vel.z > 0)
            while (hitCollision(character)) character.z+= 0.01;
         else if (vel.z < 0)
            while (hitCollision(character)) character.z-= 0.01;*/
       }
   }
   else {
      if (!timeWaiting) {
         character.y -= .05;
         if (hitCollision(character)) {
            
            character.x += character.vel.x * dt;
            character.y += character.vel.y * dt;
            character.z += character.vel.z * dt;
         } 
         character.y += .05;
      }
      else {
         if (timeWaiting > WAIT_TIME) {
            timeWaiting = 0;
            x = nodes[0].x;
            y = nodes[0].y;
            z = nodes[0].z;
            visible = true;
            visibleTimer = WAIT_TIME/5;
            oldTimer = 0;
            curDest = 0;  
         }
         else {
            if (visibleTimer + oldTimer < timeWaiting) {
               visible = !visible;
               visibleTimer *= .8;
               oldTimer = timeWaiting;
            }
            timeWaiting++;
         }
      }
   }
}

int MovingObj::checkTarget(){
   for (unsigned int i = 0; i < objs.size(); i++){//scan all objects
      if (objs[i] != this){
         if (objs[i]->hitCollision(*this)){
            if(objs[i]->classId != MOVINGOBJ_ID && objs[i]->classId != GOBJ_ID)
              return 1;//hit
         }
      }
   }
   return 0;//no problem
}

void MovingObj::addNode(pnt3d new_node, float speed){
   nodes.push_back(new_node);
   speeds.push_back(speed);
}

void MovingObj::click(float cx, float cy, float cz) {
   if (!controllable || timeWaiting > 1) {
      return;
   }
      
   if (controlled){
      controlled = false;
      timeWaiting++;
   } else {
      character.addEnergy(-1);
      character.control(this);
      controlled = true;
      timeWaiting = 0;
      vel.x = 0;
      vel.y = 0;
      vel.z = 0;
   }
}
