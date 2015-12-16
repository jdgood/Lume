#include "obj.h"

void obj::setColor(int s, int n){
   if ((int)surface[s].overts.size() > n){   
      int c = surface[s].overts[n];
      if (c > 8) c-= 8;
      glColor3f(waveEffect[c]*colors[c].x + colors[c].x/5,
                waveEffect[c]*colors[c].y + colors[c].y/5,
                waveEffect[c]*colors[c].z + colors[c].z/5);
      return;
   }
   glColor3f(1,1,1);
}

void setTexCoord(int i){
   if(i == 0){
      glTexCoord2f(0,1);
   }
   else if(i == 1){
       glTexCoord2f(1,1);
   }
   else if(i == 2){
      glTexCoord2f(1,0);
   }
   else{
      glTexCoord2f(0,0);
   }
}

void setTexCoord(int s,int i,int w, int h,int l){
   if (w < 1) w = 1;
   if (h < 1) h = 1;
   if (l < 1) l = 1;
   int a, b;
   a = b = 0;
   //a = w;
   //b = h;   
   switch (s){
      case 0:case 1:
      a = h;
      b = w;      
      break;
      case 2:case 3:
      a = l;
      b = w;      
      break;      
      case 4:case 5:
      a = l;
      b = h;
      break;
   }
   if(i == 0){//f-0 bk-1 t-2 bt-3 l-4 r-5
      glTexCoord2f(0,0);
   }
   else if(i == 1){
       glTexCoord2f(a,0);
   }
   else if(i == 2){
      glTexCoord2f(a,b);
   }
   else{
      glTexCoord2f(0,b);
   }
}

void obj::draw() {
   if ((!visible && !wireframe) || hidden) return;
   if (classId != RAMP_ID) glEnable(GL_CULL_FACE);
   glPushMatrix();
     glTranslatef(x, y, z);
     glScalef(sf.x,sf.y,sf.z);
     for (unsigned int i = 0; i < surface.size(); i++){//loop through surfaces         
         glLoadName(index);//used for picking 
         if (basic || wireframe) glBindTexture(GL_TEXTURE_2D,-1);
         if (wireframe){glBegin(GL_LINE_LOOP);}
         else {
            if(classId == TELEPORT_ID){
               glBindTexture(GL_TEXTURE_2D, (i != 2)?texture.id:1);
            } 
            else if (classId == GROUND_ID) {
               glBindTexture(GL_TEXTURE_2D, (i != 2)?texture.id:1);
            }
         	else{
         		glBindTexture(GL_TEXTURE_2D, (i != 2  || (classId != OBJ_ID))?texture.id:1);//&& (classId != GROUND_ID
         	}
         	glBegin(GL_POLYGON);
        	}
            glNormal3fv(surface[i].n);
               for (unsigned int j = 0; j < surface[i].verts.size(); j++){//loop through surfaces
                  //(classId != 5) ? setTexCoord(i,j,(xmax-xmin)/4,(ymax-ymin)/4,(zmax-zmin)/4) : setTexCoord(j);
                  setTexCoord(j);

                  // takes sin effect off the skybox
                  if (!(classId == SKY_ID)) setColor(i,j);
                  else glColor3f(1,1,1);
                  if (basic && !wireframe) glColor3f(0,0,0);
                  if (wireframe) {
                     glColor3f((glowBrightness)*(edgeColor.x+((float)(rand()%5)/40.0)),//wireFrameBrightness
                              (glowBrightness)*(edgeColor.y+((float)(rand()%5)/40.0)),
                              (glowBrightness)*(edgeColor.z+((float)(rand()%5)/40.0)));
                  }
                  glVertex3f(surface[i].verts[j].x,surface[i].verts[j].y,surface[i].verts[j].z);
               }
         glEnd(); 
     }
   glPopMatrix();
   glDisable(GL_CULL_FACE);
}

int obj::getOVert(float cx, float cy, float cz){
   cx -= x;
   cy -= y;
   cz -= z;
   if (sqrt((cx - xmin)*(cx - xmin)) < sqrt((cx - xmax)*(cx - xmax))){ //left
      if (sqrt((cy - ymin)*(cy - ymin)) < sqrt((cy - ymax)*(cy - ymax))){ //bottom
         if (sqrt((cz - zmin)*(cz - zmin)) < sqrt((cz - zmax)*(cz - zmax))){ //front
            return 0;
         } else { //back
            return 4;
         }
      } else { //top
         if (sqrt((cz - zmin)*(cz - zmin)) < sqrt((cz - zmax)*(cz - zmax))){ //front
            return 2;
         } else { //back
            return 6;
         }
      }
   } else { //right
      if (sqrt((cy - ymin)*(cy - ymin)) < sqrt((cy - ymax)*(cy - ymax))){ //bottom
         if (sqrt((cz - zmin)*(cz - zmin)) < sqrt((cz - zmax)*(cz - zmax))){ //front
            return 1;
         } else { //back
            return 5;
         }
      } else { //top
         if (sqrt((cz - zmin)*(cz - zmin)) < sqrt((cz - zmax)*(cz - zmax))){ //front
            return 3;
         } else { //back
            return 7;
         }
      }
   }
   return 0;
}

void obj::fadeColor(){
   int j = 0;   
   for (unsigned int i = 0; i < colors.size(); i++){
      if (colors[i].x < .8) {colors[i].x += .001; j++;}
      if (colors[i].y < .8) {colors[i].y += .001; j++;}
      if (colors[i].z < .8) {colors[i].z += .001; j++;}
      if (colors[i].x > .9) {colors[i].x -= .001; j++;}
      if (colors[i].y > .9) {colors[i].y -= .001; j++;}
      if (colors[i].z > .9) {colors[i].z -= .001; j++;}
   }
   if (!j) { 
      colored = 0;//no change made
      //litColor == -1;
      for (unsigned int i = 0; i < colors.size(); i++){
         colors[i].x = colors[i].y = colors[i].z = .8;
      }
   }
}

void obj::update(float dt){
   if (texture.frame != 0)
   	texture.update(dt);
   if (colored > 1) colored--;
   if (colored == 1) fadeColor();

}

void obj::fullColor(){//called when stepped on
   if (litColor == -1) {//is zero  
      litColor = rand()%6;
   }
   
   for (unsigned int i = 0; i < colors.size(); i++){
      colors[i].x = edgeColor.x - litColor*.1;
      colors[i].y = edgeColor.y - litColor*.1;
      colors[i].z = edgeColor.z - litColor*.1;
      /*switch(litColor){
      case 0:
         colors[i].x = 0;
         colors[i].y = 1;
         colors[i].z = 1; 
         break;
      case 1:
         colors[i].x = 1;
         colors[i].y = 1;
         colors[i].z = 0; 
         break;
      case 2:
         colors[i].x = 0;
         colors[i].y = 1;
         colors[i].z = 0.2; 
         break;
      case 3:
         colors[i].x = 0.6;
         colors[i].y = 0;
         colors[i].z = 1; 
         break;
      case 4:
         colors[i].x = 1;
         colors[i].y = 0.4;
         colors[i].z = 0; 
         break;
      case 5:
         colors[i].x = 1;
         colors[i].y = 0;
         colors[i].z = 0.2; 
         break;
      }*/
   } 
   colored=20;//set delay before decay 
}

void obj::click(float cx, float cy, float cz){
   /*int i = getOVert(cx,cy,cz);
   if (i < colors.size()){      
      colors[i].x -= 0.4;
      colors[i].y -= 0.4;
      colors[i].z -= 0.4;
      switch(rand()%3){
         case 0:
            colors[i].x += 0.4;
            break;
         case 1:
            colors[i].y += 0.4;
            break;
         case 2:
            colors[i].z += 0.4;
            break;
      }
   }*/
}

bool obj::hitTest(float ix, float iy, float iz){
   float dx = (ix-x);
   float dy = (iy-y);
   float dz = (iz-z);
   if (dx < xmin || dx > xmax) return false;
   if (dy < ymin || dy > ymax) return false;
   if (dz < zmin || dz > zmax) return false;
   if (dy <= fx*(dx) + fz*(dz) + fy) return true;
   return false;
}

bool obj::hitCamera(float ix, float iy, float iz){
   float dx = (ix-x);
   float dy = (iy-y);
   float dz = (iz-z);
   float cameraOffset = .2;
   if (dx < xmin-cameraOffset || dx > xmax+cameraOffset) return false;
   if (dy < ymin-cameraOffset || dy > ymax+cameraOffset) return false;
   if (dz < zmin-cameraOffset || dz > zmax+cameraOffset) return false;
   if (dy <= fx*(dx) + fz*(dz) + fy + (cameraOffset * 2)) return true;
   return false;
}

bool obj::hitCollision(obj object){

  if((x + xmax) <= (object.x + object.xmin)){
    return false;
  }
  else if((x + xmin) >= (object.x + object.xmax)){
    return false;
  }
  else if((y + ymax) <= (object.y + object.ymin)){
    return false;
  }
  else if((y + ymin) >= (object.y + object.ymax)){
    return false;
  }
  else if((z + zmax) <= (object.z + object.zmin)){
    return false;
  }
  else if((z + zmin) >= (object.z + object.zmax)){
    return false;
  }
  if (object.y + object.ymin - y <= fx*(object.x - x) + fz*(object.z - z) + fy) return true;
  return false;
}

pnt3d obj::getNorm(float ix, float iy, float iz){
   pnt3d s = pnt3d();
   s.x = 1;
   float temp;
   float test = (ix - x - xmax);
   temp = (iy - y - ymax);
   if (temp*temp < test*test) {
      s.x = 0; s.y = 1;
      test = temp;
   }
   temp = (iz - z - zmax);
   if (temp*temp < test*test) {
      s.x = 0; s.y = 0; s.z = 1;
      test = temp;
   }
   temp = (ix - x - xmin);
   if (temp*temp < test*test) {
      s.x = -1; s.y = 0; s.z = 0;
      test = temp;
   }
   temp = (iy - y - ymin);
   if (temp*temp < test*test) {
      s.x = 0; s.y = -1; s.z = 0;
      test = temp;
   }
   temp = (iz - z - zmin);
   if (temp*temp < test*test) {
      s.x = 0; s.y = 0; s.z = -1;
   }
   return s;
}

void addCube(obj* o,float dx,float dy,float dz){
   if (dx < 0) dx *=-1; 
   if (dy < 0) dy *=-1;
   if (dz < 0) dz *=-1; 
   dx/=2;
   dy/=2;
   dz/=2;
   surf s = surf();//front
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = 1*dz;
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   //obect verts
   s.overts.push_back(6);
   s.overts.push_back(4);
   s.overts.push_back(5);
   s.overts.push_back(7);
   o->surface.push_back(s);
   s = surf();//back
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = -1*dz;
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   //obect verts
   s.overts.push_back(3);
   s.overts.push_back(1);
   s.overts.push_back(0);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//top
   s.n[0] = 0;
   s.n[1] = 1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   //obect verts
   s.overts.push_back(2);
   s.overts.push_back(6);
   s.overts.push_back(7);
   s.overts.push_back(3);
   o->surface.push_back(s);
   s = surf();//bottom
   s.n[0] = 0;
   s.n[1] = -1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   //obect verts
   s.overts.push_back(1);
   s.overts.push_back(5);
   s.overts.push_back(4);
   s.overts.push_back(0);
   o->surface.push_back(s);
   s = surf();//left
   s.n[0] = -1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   //obect verts
   s.overts.push_back(6);
   s.overts.push_back(2);
   s.overts.push_back(0);
   s.overts.push_back(4);
   o->surface.push_back(s);
   s = surf();//right
   s.n[0] = 1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   //obect verts
   s.overts.push_back(5);
   s.overts.push_back(1);
   s.overts.push_back(3);
   s.overts.push_back(7);
   o->surface.push_back(s);
   //set boundaries
   o->fx = 0;//top plane of object
   o->fy = (dy>0)?dy:0; 
   o->fz = 0;
   o->xmax = dx;//boundaries (max)
   o->ymax = dy; 
   o->zmax = dz;
   o->xmin = -dx;//boundaries (min)
   o->ymin = -dy; 
   o->zmin = -dz;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
}

obj* newCube(float x,float y,float z,float dx,float dy,float dz){
   obj* o = new obj(x,y,z);
   o->texture.load((char *)"Wave", 11);
   o->texture.play(PLAY_LOOP);
   for (int i = 0; i < 8; i++){
      o->colors.push_back(pnt3d(.8,.8,.8));
   }
   addCube(o,dx,dy,dz);
   return o;
}

obj* newCube(float x,float y,float z,float dx,float dy,float dz, char* tname){
   obj* o = new obj(x,y,z);
   if(tname) {
      if(!strncmp(tname,(char *)"textures/sprite-", 16)){
         char* sname = (char*)malloc(40*sizeof(char));
         char tempname[40];
         int frames;
         sscanf(tname, "textures/sprite-%d-%s", &frames, tempname);
         sname = strtok(tempname, ".");
         //printf("%s\n", sname);
         o->texture.load(sname, frames);
      }
      else if(!strcmp(tname,(char *)"textures/concrete.jpg")){
         int random = rand()%4;
         if (random == 0)
            o->texture.load((char*)"textures/Tex_24_grayscale.jpg");
         if (random == 1)
            o->texture.load((char*)"textures/Tex_24_grayscale.jpg");
         if (random == 2)
            o->texture.load((char*)"textures/Tex_21_grayscale.jpg");
         if (random == 3)
            o->texture.load((char*)"textures/Tex_21_grayscale.jpg");
        //o->texture.load(tname);
      }
      else{
         o->texture.load(tname);
      }
      for (int i = 0; i < 8; i++){
         o->colors.push_back(pnt3d(.8,.8,.8));
      }
   } else {
      o->classId = GLASS_ID;
   }
   addCube(o,dx,dy,dz);
   return o;
}

void addRamp1(obj* o,float dx,float dy,float dz){
   if (dx < 0) dx *=-1; 
   if (dy < 0) dy *=-1;
   if (dz < 0) dz *=-1; 
   dx/=2;
   dy/=2;
   dz/=2;
   surf s = surf();//back
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = 1*dz;
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   //obect verts
   s.overts.push_back(4);
   s.overts.push_back(5);
   s.overts.push_back(7);
   o->surface.push_back(s);
   s = surf();//front
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = -1*dz;
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   
   //obect verts
   s.overts.push_back(0);
   s.overts.push_back(1);
   s.overts.push_back(3);
   o->surface.push_back(s);
   s = surf();//top
   s.n[0] = 0;
   s.n[1] = 1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   //obect verts
   s.overts.push_back(0);
   s.overts.push_back(4);
   s.overts.push_back(7);
   s.overts.push_back(3);
   o->surface.push_back(s);
   s = surf();//bottom
   s.n[0] = 0;
   s.n[1] = -1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   //obect verts
   s.overts.push_back(1);
   s.overts.push_back(5);
   s.overts.push_back(4);
   s.overts.push_back(0);
   o->surface.push_back(s);
   s = surf();//right
   s.n[0] = 1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   //obect verts
   s.overts.push_back(5);
   s.overts.push_back(1);
   s.overts.push_back(3);
   s.overts.push_back(7);
   o->surface.push_back(s);
   //set boundaries
   o->fx = dy/dx;//top plane of object
   o->fy = 0; 
   o->fz = 0;
   o->xmax = dx;//boundaries (max)
   o->ymax = dy; 
   o->zmax = dz;
   o->xmin = -dx;//boundaries (min)
   o->ymin = -dy; 
   o->zmin = -dz;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
}

void addRamp2(obj* o,float dx,float dy,float dz){
   if (dx < 0) dx *=-1; 
   if (dy < 0) dy *=-1;
   if (dz < 0) dz *=-1; 
   dx/=2;
   dy/=2;
   dz/=2;
   surf s = surf();//back
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = 1*dz;
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,dy,dz));//6

   //obect verts
   s.overts.push_back(5);
   s.overts.push_back(4);
   s.overts.push_back(6);
   o->surface.push_back(s);
   s = surf();//front
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = -1*dz;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   //obect verts
   s.overts.push_back(1);
   s.overts.push_back(0);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//top
   s.n[0] = 0;
   s.n[1] = 1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   
   //obect verts
   s.overts.push_back(1);
   s.overts.push_back(5);
   s.overts.push_back(6);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//bottom
   s.n[0] = 0;
   s.n[1] = -1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   //obect verts
   s.overts.push_back(1);
   s.overts.push_back(5);
   s.overts.push_back(4);
   s.overts.push_back(0);
   o->surface.push_back(s);
   s = surf();//left
   s.n[0] = -1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   //obect verts
   s.overts.push_back(6);
   s.overts.push_back(2);
   s.overts.push_back(0);
   s.overts.push_back(4);
   o->surface.push_back(s);
   //set boundaries
   o->fx = -dy/dx;//top plane of object
   o->fy = 0; 
   o->fz = 0;
   o->xmax = dx;//boundaries (max)
   o->ymax = dy; 
   o->zmax = dz;
   o->xmin = -dx;//boundaries (min)
   o->ymin = -dy; 
   o->zmin = -dz;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
}

void addRamp3(obj* o,float dx,float dy,float dz){
   if (dx < 0) dx *=-1; 
   if (dy < 0) dy *=-1;
   if (dz < 0) dz *=-1; 
   dx/=2;
   dy/=2;
   dz/=2;
   surf s = surf();//back
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = 1*dz;
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   //obect verts
   s.overts.push_back(6);
   s.overts.push_back(4);
   s.overts.push_back(5);
   s.overts.push_back(7);
   o->surface.push_back(s);
   s = surf();//top
   s.n[0] = 0;
   s.n[1] = 1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   //obect verts
   s.overts.push_back(0);
   s.overts.push_back(1);
   s.overts.push_back(7);
   s.overts.push_back(6);
   o->surface.push_back(s);
   s = surf();//bottom
   s.n[0] = 0;
   s.n[1] = -1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   //obect verts
   s.overts.push_back(1);
   s.overts.push_back(5);
   s.overts.push_back(4);
   s.overts.push_back(0);
   o->surface.push_back(s);
   s = surf();//left
   s.n[0] = -1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,dy,dz));//6
   //obect verts
   s.overts.push_back(4);
   s.overts.push_back(0);
   s.overts.push_back(6);
   o->surface.push_back(s);
   s = surf();//right
   s.n[0] = 1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,dy,dz));//7
   
   //obect verts
   s.overts.push_back(5);
   s.overts.push_back(1);
   s.overts.push_back(7);

   o->surface.push_back(s);
   //set boundaries
   o->fx = 0;//top plane of object
   o->fy = 0; 
   o->fz = dy/dz;
   o->xmax = dx;//boundaries (max)
   o->ymax = dy; 
   o->zmax = dz;
   o->xmin = -dx;//boundaries (min)
   o->ymin = -dy; 
   o->zmin = -dz;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
}

void addRamp4(obj* o,float dx,float dy,float dz){
   if (dx < 0) dx *=-1; 
   if (dy < 0) dy *=-1;
   if (dz < 0) dz *=-1; 
   dx/=2;
   dy/=2;
   dz/=2;
   surf s = surf();//front
   s.n[0] = 0;
   s.n[1] = 0;
   s.n[2] = -1*dz;
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   //obect verts
   s.overts.push_back(3);
   s.overts.push_back(1);
   s.overts.push_back(0);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//top
   s.n[0] = 0;
   s.n[1] = 1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   //obect verts
   s.overts.push_back(4);
   s.overts.push_back(5);
   s.overts.push_back(3);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//bottom
   s.n[0] = 0;
   s.n[1] = -1*dy;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   //obect verts
   s.overts.push_back(4);
   s.overts.push_back(5);
   s.overts.push_back(3);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//left
   s.n[0] = -1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(-dx,-dy,dz));//4
   s.verts.push_back(pnt3d(-dx,-dy,-dz));//0
   s.verts.push_back(pnt3d(-dx,dy,-dz));//2
   //obect verts
   s.overts.push_back(4);
   s.overts.push_back(0);
   s.overts.push_back(2);
   o->surface.push_back(s);
   s = surf();//right
   s.n[0] = 1*dx;
   s.n[1] = 0;
   s.n[2] = 0;
   s.verts.push_back(pnt3d(dx,-dy,dz));//5
   s.verts.push_back(pnt3d(dx,-dy,-dz));//1
   s.verts.push_back(pnt3d(dx,dy,-dz));//3
   
   //obect verts
   s.overts.push_back(5);
   s.overts.push_back(1);
   s.overts.push_back(3);
   
   o->surface.push_back(s);
   //set boundaries
   o->fx = 0;//top plane of object
   o->fy = 0; 
   o->fz = -dy/dz;
   o->xmax = dx;//boundaries (max)
   o->ymax = dy; 
   o->zmax = dz;
   o->xmin = -dx;//boundaries (min)
   o->ymin = -dy; 
   o->zmin = -dz;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
}

obj* newRamp(float x,float y,float z,float dx,float dy,float dz, int rotate, int oneeighty, char* tname){
   obj* o = new obj(x,y,z);
   if(!strncmp(tname,(char *)"textures/sprite-", 16)){
      char* sname = (char*)malloc(40*sizeof(char));
      char tempname[40];
      int frames;
      sscanf(tname, "textures/sprite-%d-%s", &frames, tempname);
      sname = strtok(tempname, ".");
      //printf("%s\n", sname);
      o->texture.load(sname, frames);
   }
   else{
     o->texture.load(tname);
   }
   for (int i = 0; i < 8; i++){
      o->colors.push_back(pnt3d(.8,.8,.8));
   }
   if(rotate == 0 && oneeighty == 0){
      addRamp1(o,dx,dy,dz);
   }
   else if(rotate == 0 && oneeighty == 1){
      addRamp2(o,dx,dy,dz);
   }
   else if(rotate == 1 && oneeighty == 1){
      addRamp3(o,dx,dy,dz);
   }
   else if(rotate == 1 && oneeighty == 0){
      addRamp4(o,dx,dy,dz);
   }
   o->classId = RAMP_ID;
   return o;
}

objmodel* newModel(float x,float y,float z,float dx,float dy,float dz,char*tname,char*in_objname, int rotation){
   objmodel* o = new objmodel(x,y,z,in_objname);
   if(tname == NULL){
      o->texture.id = 0;
   }
   else{
      o->texture.load(tname);
   }
   if(!strcmp(in_objname, (char*) "models/monitor/monitor.obj")){
      o->flipy = 180;
   }
   else{
      o->flipy = 0;
   }
   if(rotation == 0){
     o->drawrotation = 0;
   }
   else if(rotation == 1){
     o->drawrotation = 90;
   }
   else if(rotation == 2){
     o->drawrotation = 180;
   }
   else if(rotation == 3){
     o->drawrotation = 270;
   }
   o->xmax = 3;
   o->ymax = 2;
   o->zmax = 1;
   o->xmin = -3;
   o->ymin = -5;
   o->zmin = -1;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
   return o;
}

void objmodel::draw(){
   if ((!visible && !wireframe) || hidden) return;
   glPushMatrix();
		glTranslatef(x,y,z);
		glRotatef(flipy, 1, 0, 0);
		glRotatef(drawrotation, 0, 1, 0);
		/*if(wireframe){
         glBegin(GL_LINE_LOOP);
         glVertex3f(xmax, ymax, zmax);
         glVertex3f(xmax, ymin, zmax);
         glVertex3f(xmin, ymin, zmax);
         glVertex3f(xmin, ymax, zmax);
         glEnd();
         glBegin(GL_LINE_LOOP);
         glVertex3f(xmax, ymax, zmin);
         glVertex3f(xmax, ymin, zmin);
         glVertex3f(xmin, ymin, zmin);
         glVertex3f(xmin, ymax, zmin);
         glEnd();
      }*/
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glBegin(GL_TRIANGLES); 
		for(unsigned int i = 0; i < om.faces.size(); i++){
		   glTexCoord2f(0,0);
		   glVertex3f(om.faces[i].vertices[0].pos.x,om.faces[i].vertices[0].pos.y,om.faces[i].vertices[0].pos.z);
		   glTexCoord2f(0,1);
		   glVertex3f(om.faces[i].vertices[1].pos.x,om.faces[i].vertices[1].pos.y,om.faces[i].vertices[1].pos.z);
		   glTexCoord2f(1,1);
		   glVertex3f(om.faces[i].vertices[2].pos.x,om.faces[i].vertices[2].pos.y,om.faces[i].vertices[2].pos.z);
		}
		glEnd();
   glPopMatrix();
}

tele* newTele(float x,float y,float z,float dx,float dy,float dz,char*tname,char*in_mapName, bool active){
   tele* o = new tele(x,y,z,in_mapName, active);
   if(tname == NULL){
      o->texture.id = 0;
   }
   else{
      o->texture.load(tname);
   }
   o->xmax = 1.5;
   o->ymax = .3;
   o->zmax = .6;
   o->xmin = -1.5;
   o->ymin = -4;
   o->zmin = -.6;
   float tMax = max(abs((o->xmax) - (o->xmin)), abs((o->ymax) - (o->ymin)));
   o->radius = max(abs(tMax), abs((o->zmax) - (o->zmin)));
   return o;
}

tele* newTele(char* in_mapName, float x, float y, float z){
   tele* o = new tele(0,0,0,in_mapName,true);
   o->texture.id = 0;
   o->doTele = true;
   return o;
}

void tele::draw(){
   glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   if ((!visible && !wireframe) || hidden) return;
   glPushMatrix();
      glTranslatef(x,y,z);
      /*if(wireframe){
         glBegin(GL_LINE_LOOP);
         glVertex3f(xmax, ymax, zmax);
         glVertex3f(xmax, ymin, zmax);
         glVertex3f(xmin, ymin, zmax);
         glVertex3f(xmin, ymax, zmax);
         glEnd();
         glBegin(GL_LINE_LOOP);
         glVertex3f(xmax, ymax, zmin);
         glVertex3f(xmax, ymin, zmin);
         glVertex3f(xmin, ymin, zmin);
         glVertex3f(xmin, ymax, zmin);
         glEnd();
      }
		else if(active){*/
		if(active){
			glColor4f(.13,.13,.5,.5);
			//glColor3f(1,1,1);
		   glBindTexture(GL_TEXTURE_2D, texture.id);
		   glBegin(GL_TRIANGLES); 
		   for(unsigned int i = 0; i < om.faces.size(); i++){
		      glTexCoord2f(0,0);
		      glVertex3f(om.faces[i].vertices[0].pos.x,om.faces[i].vertices[0].pos.y,om.faces[i].vertices[0].pos.z);
		      glTexCoord2f(0,1);
		      glVertex3f(om.faces[i].vertices[1].pos.x,om.faces[i].vertices[1].pos.y,om.faces[i].vertices[1].pos.z);
		      glTexCoord2f(1,1);
		      glVertex3f(om.faces[i].vertices[2].pos.x,om.faces[i].vertices[2].pos.y,om.faces[i].vertices[2].pos.z);
		   }
		   glEnd();
         /*char *ch;
         char text[32];
         glColor3f(1.0, 1.0, 1.0);
         glRasterPos3f(0, 0, 0.0);
         sprintf(text, "Map Name:");  
         for(ch = text; *ch; ch++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)*ch);
         }*/
   }
   glPopMatrix();
   glDisable (GL_BLEND);
}

bool tele::hitCollision(obj object){
  doTele = false;
  if((x + xmax) <= (object.x + object.xmin)){
    return false;
  }
  else if((x + xmin) >= (object.x + object.xmax)){
    return false;
  }
  else if((y + ymax) <= (object.y + object.ymin)){
    return false;
  }
  else if((y + ymin) >= (object.y + object.ymax)){
    return false;
  }
  else if((z + zmax) <= (object.z + object.zmin)){
    return false;
  }
  else if((z + zmin) >= (object.z + object.zmax)){
    return false;
  }
  if (object.y + object.ymin - y <= fx*(object.x - x) + fz*(object.z - z) + fy && active){
    //add tele stuff
    if(active && !loading){
       //printf("inhere\n");
       doTele = true;
    }
    return false;
  }
  return false;
}

/*------obj2d---------*/
obj2d::obj2d(){
   classId = OBJ2D_ID;
   n = pnt3d(rand()%100 / (float)100,rand()%100 / (float)100,rand()%100 / (float)100);
   x = 0;
   y = 0;
   z = 0;
   offset = 0;
   width = 0.10;
   height = 0.10;
   extex = NULL;
   ref = NULL;
   parent = NULL;
   lifetime = 0;
   alpha = 1;
   texture.load((char *)"textures/lightsource.jpg");
   color = pnt3d(1,1,1);
   clip = false;
   fade = false;
   resize();
   objectivenum = 0;
   fadeOutDist = 20;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(char *tex){
   classId = OBJ2D_ID;
   n = pnt3d(rand()%100 / (float)100,rand()%100 / (float)100,rand()%100 / (float)100);
   x = 0;
   y = 0;
   z = 0;
   offset = 0;
   width = 0.10;
   height = 0.10;
   extex = NULL;
   ref = NULL;
   parent = NULL;
   lifetime = 0;
   alpha = 1;
   texture.load(tex);
   color = pnt3d(1,1,1);
   clip = false;
   fade = false;
   resize();
   objectivenum = 0;
   fadeOutDist = 20;
   strcpy(objectivename, (char*)"null");
}

obj2d * newTrail(float nx, float ny, float nz,vec3d* r){
   obj2d * o = new obj2d(nx,ny,nz,r);
   o->texture.id = 13;
   o->lifetime = 200;
   o->fadeOutDist = 0;
   o->offset = 0.05;
   o->classId = FLOATER2D_ID;
   return o;
}

obj2d::obj2d(float nx, float ny, float nz,vec3d* r){
   classId = OBJ2D_ID;
   n = pnt3d(0,1,0);
   x = nx;
   y = ny;
   z = nz;
   offset = 0;
   width = 1;
   height = 1;
   ref = r;
   parent = NULL;
   extex = NULL;
   texture.id = 12;//load((char*)"sprint", 6);//3 is lume //10 is screen
   lifetime = 200;//die after some time
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
   fadeOutDist = 20;
   objectivenum = 0;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float nx, float ny, float nz,vec3d* r, int time, bool insight){ //original level up animation
   classId = OBJ2D_ID;
   n = pnt3d(0,1,0);
   x = nx;
   y = ny;
   z = nz;
   offset = 0;
   if(!insight){
     width = 1;
     height = 1;
   }
   else{
     width = 3.0;
     height = 3.0;
   }
   ref = r;
   parent = NULL;
   extex = NULL;
   if(insight){
     texture.load((char*)"textures/insightring.jpg");//3 is lume //10 is screen
   }
   else{
     texture.load((char*)"textures/ring.jpg");//3 is lume //10 is screen
   }
   lifetime = time;//die after some time
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
   objectivenum = 0;
   fadeOutDist = 20;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float mx, float my, float mz, float nx, float ny, float nz, float w, float h, vec3d* r,char* tex){
   classId = OBJ2D_ID;
   n = pnt3d(nx,ny,nz);
   x = mx;
   y = my;
   z = mz;
   parent = NULL;
   offset = 0;
   width = w;
   height = h;
   ref = r;
   extex = NULL;
   texture.load(tex);//load((char*)"sprint", 6);//3 is lume //10 is screen
   lifetime = 0;//die after some time
   alpha = 1;
   color = pnt3d(1,1,1);
   clip = true;
   resize();
   objectivenum = 0;
   fadeOutDist = 20;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float nx, float ny, float nz, obj* o,vec3d* r, float size){//nyan create
   classId = DETAIL2D_ID;
   n = o->getNorm(nx,ny,nz);
   x = nx;
   y = ny;
   z = nz;
   parent = o;
   width = size;
   height = size;
   ref = r;
   extex = NULL;
   texture.load((char*)"nyan", 12);
   lifetime = 0;
   alpha = 1;
   color = pnt3d(1, 1, 1);
   resize();
   objectivenum = 0;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float nx, float ny, float nz, obj* o,vec3d* r, char* texname, int frames, float size){//animated 2d detail texture
   classId = DETAIL2D_ID;
   n = o->getNorm(nx,ny,nz);
   x = nx;
   y = ny;
   z = nz;
   parent = o;
   width = size;
   height = size;
   ref = r;
   extex = NULL;
   if (!fullLoad) {
     frames = 2;
   }
   texture.load(texname, frames);
   texture.play(30);
   lifetime = 0;
   alpha = 1;
   fadeOutDist = 0;
   color = pnt3d(1, 1, 1);
   resize();
}

obj2d::obj2d(float nx, float ny, float nz, obj* o,vec3d* r){//normal click obj
   classId = GOBJ2D_ID;
   n = o->getNorm(nx,ny,nz);
   x = nx;
   y = ny;
   z = nz;
   fadeOutDist = 20;
   parent = o;
   //offset = offset2d;
   //offset2d += 0.001;
   width = 8;
   height = 8;
   ref = r;
   extex = NULL;
   if (fullLoad) {
      int random = rand();
      if (random%4 == 0) {
         texture.load((char*)"growth", 30);
      }
      if (random%4 == 1) {
         texture.load((char*)"growth2_", 30);
      }  
      if (random%4 == 2) {
         texture.load((char*)"growth3_", 30);
      }
      if (random%4 == 3) {
         texture.load((char*)"growth4_", 30);
      }
   }
   else {
      int random = rand();
      if (random%4 == 0) {
         texture.load((char*)"growth", 2);
      }
      if (random%4 == 1) {
         texture.load((char*)"growth2_", 2);
      }  
      if (random%4 == 2) {
         texture.load((char*)"growth3_", 2);
      }
      if (random%4 == 3) {
         texture.load((char*)"growth4_", 2);
      }
   }
   texture.play(10);
   lifetime = 0;
   alpha = 1;
   switch(rand()%6){
      case 0:
         color = pnt3d(0, 1, 1);
         break;
      case 1:
         color = pnt3d(1, 1, 0);
         break;
      case 2:
         color = pnt3d(0, 1, .2);
         break;
      case 3:
         color = pnt3d(.6, 0, 1);
         break;
      case 4:
         color = pnt3d(1, .4, 0);
         break;
      case 5:
         color = pnt3d(1, 0, .2);
         break;
   }
   resize();
   objectivenum = 0;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float nx, float ny, float nz, obj* o,vec3d* r,int texid,float size){
   classId = OBJ2D_ID;
   n = o->getNorm(nx,ny,nz);
   x = nx;
   y = ny;
   z = nz;
   parent = o;
   offset = 0;
   width = size;
   height = size;
   ref = r;
   extex = NULL;
   texture.id = texid;//load((char*)"sprint", 6);//3 is lume //10 is screen
   lifetime = 0;
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
   objectivenum = 0;
   fadeOutDist = 20;
   strcpy(objectivename, (char*)"null");
}

/**
 * Used for 
 * floating 2d objects.  Example use is building signs.  Set by
 * pressing '[' and it is placed in the character's location.
 */
obj2d::obj2d(float nx, float ny, float nz, pnt3d dir, vec3d* r,int texid,float size){
   classId = FLOATER2D_ID;
   n = dir;
   x = nx;
   y = ny;
   z = nz;
   parent = NULL;
   offset = 0;
   width = size;
   height = size;
   ref = r;
   extex = NULL;
   texture.id = texid;
   lifetime = 0;
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
   objectivenum = 0;
   fadeOutDist = 0;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float nx, float ny, float nz, obj* o,vec3d* r,int texid,float size, int i){
   classId = DETAIL2D_ID;
   n = o->getNorm(nx,ny,nz);
   x = nx;
   y = ny;
   z = nz;
   parent = o;
   //offset = offset2d;
   //offset2d += 0.001;
   width = size;
   height = size;
   ref = r;
   extex = NULL;
   texture.id = texid;//load((char*)"sprint", 6);//3 is lume //10 is screen
   lifetime = 0;
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
   objectivenum = 0;
   fadeOutDist = 10;
   strcpy(objectivename, (char*)"null");
}

obj2d::obj2d(float nx, float ny, float nz, obj* o,vec3d* r, Sprite* tex){
   classId = OBJ2D_ID;
   n = o->getNorm(nx,ny,nz);
   x = nx;
   y = ny;
   z = nz;
   parent = o;
   //offset = offset2d;
   //offset2d += 0.001;
   width = 8;
   height = 8;
   ref = r;
   extex = tex;
   lifetime = 0;//die after some time
   alpha = 1;
   color = pnt3d(1,1,1);
   resize();
   fadeOutDist = 20;
   objectivenum = 0;
   strcpy(objectivename, (char*)"null");
}

void obj2d::resize(){
   amin = 1;
   amax = 1;
   bmin = 1;
   bmax = 1;
   flip = false;
   radius = max(width,height)/2;
   float tmp;
   if (!parent) return;  
   if (n.z) {
      tmp = 2*(x - parent->x - parent->xmin);
      if (width > tmp) {
         (n.z < 0) ? amax = tmp/width : amin = tmp/width;
      }
      tmp = 2*(parent->x + parent->xmax - x);
      if (width > tmp) {
         (n.z < 0) ? amin = tmp/width : amax = tmp/width;
      }
      tmp = 2*(y - parent->y - parent->ymin);
      if (height > tmp) {
         (n.z < 0) ? bmax = tmp/height : bmin = tmp/height;
         
         if (n.z > 0){
            flip = true;
            //float tmp = bmin;
            //bmin = 1 - bmin;
            //bmax = 1 - bmax; 
         }
         //printf("3bmax:%f bmin:%f\n",bmax,bmin);
      }
      tmp = 2*(parent->y + parent->ymax - y);
      if (height > tmp) {
         (n.z < 0) ? bmin = tmp/height : bmax = tmp/height;
         
         if (n.z > 0){
            flip = true;
            //float tmp = bmin;
            //bmin = 1 - bmin;
            //bmax = 1 - bmax; 
         }
         //printf("4bmax:%f bmin:%f\n",bmax,bmin);         
         //bmax = tmp/height;
      }
   } else if (n.x) {//z y 
      tmp = 2*(z - parent->z - parent->zmin);
      if (width > tmp) {
         (n.x > 0) ? amax = tmp/width : amin = tmp/width;
      }
      tmp = 2*(parent->z + parent->zmax - z);
      if (width > tmp) {
         (n.x > 0) ? amin = tmp/width : amax = tmp/width;
      }
      tmp = 2*(y - parent->y - parent->ymin);
      if (height > tmp) {
         bmax = tmp/height;
      }
      tmp = 2*(parent->y + parent->ymax - y);
      if (height > tmp) {
         bmin = tmp/height;
      }
   } else if (n.y) {//x z
      tmp = 2*(x - parent->x - parent->xmin);
      if (width > tmp) {
         (n.y > 0) ? amax = tmp/width : amin = tmp/width;
      }
      tmp = 2*(parent->x + parent->xmax - x);
      if (width > tmp) {
         (n.y > 0) ? amin = tmp/width : amax = tmp/width;
      }
      tmp = 2*(z - parent->z - parent->zmin);
      if (height > tmp) {
         (n.y > 0) ? bmax = tmp/height : bmin = tmp/height;
      }
      tmp = 2*(parent->z + parent->zmax - z);
      if (height > tmp) {
         (n.y > 0) ? bmin = tmp/height : bmax = tmp/height;
      }
   }
   //scale      
   /*if (width < height){
      height = width;
   } else {
      width = height;
   }*/
}

float obj2d::distance(){
   if (!ref) return 1;
   float a = ref->px + ref->x - x;
   float b = ref->py + ref->y - y;
   float c = ref->pz + ref->z - z;
   a = sqrt(a*a + b*b + c*c);
   if (a < 1) return 1;
   return a;
}

void obj2d::rotateToNormal(){
	if(classId == BB2D_ID){
	  glRotatef(acos(-n.z)*RAD2DEG, n.y, -n.x, 0);
	}
	else{
	   glRotatef(acos(-n.z)*RAD2DEG, n.y, -n.x, 0);
      if (n.z == 1 || n.z == -1) {
         glRotatef(180, 0, 1, 0);
         glRotatef(180, 1, 0, 0);
      }
      else {
         glRotatef(180, 0, 0, 1);
      }
   }
}

float max(float a, float b){
   if (a > b) return a;
   return b;
}

float min(float a, float b){
   if (a < b) return a;
   return b;
}

void obj2d::update(float dt){
   //fade out with distance  
   if (fade && fadeOutDist > 0 && classId != FOG_ID && lifetime == 0){
      float d = distance();
      alpha = 1 - min(1.0, max(0,(float)(d-fadeOutDist)/(fadeOutDist)));
   }

   //level ups
   if (!parent && lifetime > 0 && lifetime < 200){
      if (classId == FLOATER2D_ID) {
         width += 0.05;
         height += 0.05;
         alpha -= 0.05;
      } else {
         width += 0.02;
         height += 0.02;
         offset += 0.1;
         alpha -= 0.05;
      }

   }
   
   //texture and lifetime update
   if (!extex) texture.update(dt);
   if (lifetime > 0){
       lifetime-=dt;
       if (lifetime < 50) alpha-= dt/50;
       if (lifetime < 1) kill();
   }
}

void obj2d::setTexCoord(int i){
   if(i == 0){
      (n.z > 0) ? glTexCoord2f((1+amax)/2,(1+bmax)/2) : glTexCoord2f((1+amax)/2,(1-bmin)/2);
      if (flip)
         glTexCoord2f((1+amax)/2,(1-bmin)/2);
   }
   else if(i == 1){
      (n.z > 0) ? glTexCoord2f((1-amin)/2,(1+bmax)/2) : glTexCoord2f((1-amin)/2,(1-bmin)/2);
      if (flip)
         glTexCoord2f((1-amin)/2,(1-bmin)/2);
   }
   else if(i == 2){
      (n.z > 0) ? glTexCoord2f((1-amin)/2,(1-bmin)/2) : glTexCoord2f((1-amin)/2,(1+bmax)/2);
      if (flip)
         glTexCoord2f((1-amin)/2,(1+bmax)/2);
   }
   else{
      (n.z > 0) ? glTexCoord2f((1+amax)/2,(1-bmin)/2) : glTexCoord2f((1+amax)/2,(1+bmax)/2);
      if (flip)
         glTexCoord2f((1+amax)/2,(1+bmax)/2);
   }
}

void obj2d::draw(){
   glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glPushMatrix();
      glTranslatef(x + n.x*(1+((int)distance()/10))*offset, y + n.y*(1+((int)distance()/10))*offset, z + n.z*(1+((int)distance()/10))*offset);
      rotateToNormal();
      glLoadName(-1);
      if (wireframe){}//glBegin(GL_LINE_LOOP);}
      else {
         glBindTexture(GL_TEXTURE_2D, (extex)?extex->id:texture.id);
         glBegin(GL_POLYGON);
         glColor4f(color.x,color.y,color.z,alpha);
         setTexCoord(0);
         glVertex3f(amax*width/2,-bmin*height/2,0);
         setTexCoord(1);
         glVertex3f(-amin*width/2,-bmin*height/2,0);
         setTexCoord(2);
         glVertex3f(-amin*width/2,bmax*height/2,0);
         setTexCoord(3);
         glVertex3f(amax*width/2,bmax*height/2,0);
         glEnd();
      }
   glPopMatrix();
   glDisable(GL_BLEND);
}

void obj2d::kill(){
   classId = -1;
}
	
pnt3d pnt3d::normalize(pnt3d self) { 
	return self / self.mag(); 
}

float pnt3d::dotproduct(pnt3d o) { 
	return x*o.x + y*o.y + z*o.z; 
}

pnt3d pnt3d::crossproduct(pnt3d o) {
	return pnt3d(y*o.z - z*o.y,
				  z*o.x - x*o.z,
				  x*o.y - y*o.x);
}

float pnt3d::distBetween(pnt3d o) {
	return sqrt((o.x-x)*(o.x-x) + (o.y-y)*(o.y-y) + (o.z-z)*(o.z-z));
}
