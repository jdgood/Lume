#include "gobj.h"

void GrowingObj::click(float cx,float cy,float cz){
   pnt3d dir = getNorm(cx,cy,cz);
   growing = true;
   if (dir.x < 0){
      if (xmin*2 > -character.buildlvl-1) gx = xmin-.5;
   } else if (dir.x > 0) {
      if (xmax*2 < character.buildlvl+1)gx = xmax+.5;
   }
   if (dir.y < 0){
      if (ymin*2 > -character.buildlvl)gy = ymin-.5;
   } else if (dir.y > 0) {
      if (ymax*2 < character.buildlvl)gy = ymax+.5;
   }
   if (dir.z < 0){
      if (zmin*2 > -character.buildlvl-1) gz = zmin-.5;
   } else if (dir.z > 0) {
      if (zmax*2 < character.buildlvl+1) gz = zmax+.5;
   }
}

float grow(float f){
   if (f < 0){
      return -1.0;
   } else if (f > 0){
      return 1.0;
   }
   return 0.0;
}

void GrowingObj::ungrow(float dt){
   if (dir.x < 0){
      for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
         for (unsigned int j = 0; j < surface[i].verts.size(); j++){
            surface[i].verts[j].x -= grow(surface[i].verts[j].x)*GROWSPEED*dt;//expand whole cube
         }
      }
      xmax-=GROWSPEED*dt;//expand BB
      xmin+=GROWSPEED*dt;
      x+=GROWSPEED*dt;//set center
   } else if (dir.x > 0) {//grow left
      for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
         for (unsigned int j = 0; j < surface[i].verts.size(); j++){
            surface[i].verts[j].x -= grow(surface[i].verts[j].x)*GROWSPEED*dt;
         }
      }
      xmax-=GROWSPEED*dt;
      xmin+=GROWSPEED*dt;
      x-=GROWSPEED*dt;
   }
   if (dir.y < 0){//grow down
      for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
         for (unsigned int j = 0; j < surface[i].verts.size(); j++){
            surface[i].verts[j].y -= grow(surface[i].verts[j].y)*GROWSPEED*dt;
         }
      }
      ymax-=GROWSPEED*dt;
      fy = ymax;
      ymin+=GROWSPEED*dt;
      y+=GROWSPEED*dt;
   } else if (dir.y > 0) {//grow up
      for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
         for (unsigned int j = 0; j < surface[i].verts.size(); j++){
            surface[i].verts[j].y -= grow(surface[i].verts[j].y)*GROWSPEED*dt;
         }
      }
      ymax-=GROWSPEED*dt;
      fy = ymax;
      ymin+=GROWSPEED*dt;
      y-=GROWSPEED*dt;
   }
   if (dir.z < 0){//grow front
      for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
         for (unsigned int j = 0; j < surface[i].verts.size(); j++){
            surface[i].verts[j].z -= grow(surface[i].verts[j].z)*GROWSPEED*dt;
         }
      }
      zmax-=GROWSPEED*dt;
      zmin+=GROWSPEED*dt;
      z+=GROWSPEED*dt;
   } else if (dir.z > 0) {//grow back
      for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
         for (unsigned int j = 0; j < surface[i].verts.size(); j++){
            surface[i].verts[j].z -= grow(surface[i].verts[j].z)*GROWSPEED*dt;
         }
      }
      zmax-=GROWSPEED*dt;
      zmin+=GROWSPEED*dt;
      z-=GROWSPEED*dt;
   }
   if (xmin > 0 || xmax < 0 || ymin > 0 || ymax < 0 || zmin > 0 || zmax < 0){
      dir = pnt3d(0,0,0);
      world.remove(index);
   }
}

void GrowingObj::update(float dt){
   texture.update(dt);
   if (colored > 1) colored--;
   if (colored == 1) fadeColor();

   if (classId<0) {ungrow(dt);return;}
   if (growing){
      growing = false;
      if (gx < xmin){//grow right
         for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
            for (unsigned int j = 0; j < surface[i].verts.size(); j++){
               surface[i].verts[j].x += grow(surface[i].verts[j].x)*GROWSPEED*dt;//expand whole cube
            }
         }
         xmax+=GROWSPEED*dt;//expand BB
         xmin-=GROWSPEED*dt;
         x-=GROWSPEED*dt;//set center
         growing = true;
         while (hitCollision(character)){
            character.x-= .01;
         }
      } else if (gx > xmax) {//grow left
         for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
            for (unsigned int j = 0; j < surface[i].verts.size(); j++){
               surface[i].verts[j].x += grow(surface[i].verts[j].x)*GROWSPEED*dt;
            }
         }
         xmax+=GROWSPEED*dt;
         xmin-=GROWSPEED*dt;
         x+=GROWSPEED*dt;
         growing = true;
         while (hitCollision(character)){
            character.x+= .01;
         }
      }
      if (gy < ymin){//grow down
         for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
            for (unsigned int j = 0; j < surface[i].verts.size(); j++){
               surface[i].verts[j].y += grow(surface[i].verts[j].y)*GROWSPEED*dt;
            }
         }
         ymax+=GROWSPEED*dt;
         fy = ymax;
         ymin-=GROWSPEED*dt;
         y-=GROWSPEED*dt;
         growing = true;
      } else if (gy > ymax) {//grow up
         for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
            for (unsigned int j = 0; j < surface[i].verts.size(); j++){
               surface[i].verts[j].y += grow(surface[i].verts[j].y)*GROWSPEED*dt;
            }
         }
         ymax+=GROWSPEED*dt;
         fy = ymax;
         ymin-=GROWSPEED*dt;
         y+=GROWSPEED*dt;
         growing = true;
      }
      if (gz < zmin){//grow front
         for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
            for (unsigned int j = 0; j < surface[i].verts.size(); j++){
               surface[i].verts[j].z += grow(surface[i].verts[j].z)*GROWSPEED*dt;
            }
         }
         zmax+=GROWSPEED*dt;
         zmin-=GROWSPEED*dt;
         z-=GROWSPEED*dt;
         growing = true;
         while (hitCollision(character)){
            character.z-= .01;
         }
      } else if (gz > zmax) {//grow back
         for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces
            for (unsigned int j = 0; j < surface[i].verts.size(); j++){
               surface[i].verts[j].z += grow(surface[i].verts[j].z)*GROWSPEED*dt;
            }
         }
         zmax+=GROWSPEED*dt;
         zmin-=GROWSPEED*dt;
         z+=GROWSPEED*dt;
         growing = true;
         while (hitCollision(character)){
            character.z+= .01;
         }
      }
      float tMax = max(abs((xmax) - (xmin)), abs((ymax) - (ymin)));
      radius = max(abs(tMax), abs((zmax) - (zmin)));
      world.update(index);
   }
   //printf("%f\n",radius);
   //printf("%f:%f:%f,%f:%f:%f\n",xmin,ymin,zmin,xmax,ymax,zmax);
}

void GrowingObj::resize(){
   for (unsigned int i = 0; i < surface.size(); i++){
      for (unsigned int j = 0; j < surface[i].verts.size(); j++){
         (surface[i].verts[j].x < 0) ? surface[i].verts[j].x = xmin : surface[i].verts[j].x = xmax;
         (surface[i].verts[j].y < 0) ? surface[i].verts[j].y = ymin : surface[i].verts[j].y = ymax;
         (surface[i].verts[j].z < 0) ? surface[i].verts[j].z = zmin : surface[i].verts[j].z = zmax;
      }
   }
}

GrowingObj* newGrowingCube(float x,float y,float z,float dx,float dy,float dz){
   float sx = (dx)?.01:2;
   float sy = (dy)?.01:1;
   float sz = (dz)?.01:2;

   GrowingObj* o = new GrowingObj(x,y,z); 
   o->dir = pnt3d(dx,dy,dz);
   o->texture.load((char*)"textures/growingobj.bmp");
   //o->texture.load((char *)"flourish", 18);
   for (int i = 0; i < 8; i++){
      o->colors.push_back(pnt3d(1,1,1));
   }
   addCube(o,sx,sy,sz);
   o->gx = (sx+dx*2)/2;
   o->gy = (sy+dy*2)/2;
   o->gz = (sz+dz*2)/2;
   //printf("%f:%f:%f,%f:%f\n",o->gx,o->gy,o->gz,o->zmin,o->zmax);
   return o;
}

