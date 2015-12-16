#include "cam.h"

cam::cam(){
  pos = vec3d(0,2,5,0,0,0);
  theta = 0;
  phi = 0;
  offset = CAM_UP;
  cinematic = false;
  cine = 0;
  zoomlevel = 0;
  currentPathIndex = 0;
  start = objective_start;
  pathFinished = false;
  if(cam_path.size() > 0){
    finish = *cam_path[0];
  }
  else{
    finish = character_start;
  } 
}

cam::cam(float* plx, float* ply, float* plz){
  px = plx;
  py = ply;  
  pz = plz;
  pos = vec3d(0,2,5,*px,*py,*pz);
  theta = 0;
  phi = 0;
  offset = CAM_UP;
  cinematic = false;
  cine = 0;
  zoomlevel = 0;
  currentPathIndex = 0;
  start = objective_start;
  pathFinished = false;
  if(cam_path.size() > 0){
    finish = *cam_path[0];
  }
  else{
    finish = character_start;
  } 
}

cam::cam(float px, float py, float pz, float x, float y, float z){
  pos = vec3d(x,y,z,px,py,pz);
  theta = 0;
  phi = 0;
  offset = CAM_UP;
  cinematic = false;
  cine = 0;
  zoomlevel = 0;
  currentPathIndex = 0;
  start = objective_start;
  pathFinished = false;
  if(cam_path.size() > 0){
    finish = *cam_path[0];
  }
  else{
    finish = character_start;
  } 
}

void cam::lookAt(float* plx, float* ply, float* plz){
  px = plx;
  py = ply;  
  pz = plz;
}

void cam::reset(){
  pos = vec3d(0,2,5,0,0,0);
  theta = 0;
  phi = 0;
  offset = CAM_UP;
  cinematic = false;
  cine = 0;
}

pnt3d cam::upVector(){
   pnt3d p= pnt3d();
   float mag = pos.mag();
   float dxz = sqrt(pos.x*pos.x + pos.z*pos.z);
   p.x = -pos.x*pos.y/(dxz*mag);
   p.y = dxz/mag;
   p.z = -pos.z*pos.y/(dxz*mag);
   return p;
}

pnt3d cam::pickPos(){
   pnt3d p = upVector();
   p = p*3;
   p.x += pos.px;
   p.y += pos.py;
   p.z += pos.pz;
   return p;
}

void cam::updateCinematic(){
   float r = pos.mag();
   pnt3d cameraVelocity;
   if(abs(*px - cinematicGoal.x) > 1 || abs(*py - cinematicGoal.y) > 1 || abs(*pz - cinematicGoal.z) > 1){
     cameraVelocity = pnt3d().normalize(pnt3d(cinematicGoal.x - *px , cinematicGoal.y - *py, cinematicGoal.z - *pz)) * .5;
   }
   else{
     cameraVelocity = pnt3d(0,0,0);
   }
   *px = cameraVelocity.x + *px;
   *py = cameraVelocity.y + *py;
   *pz = cameraVelocity.z + *pz;
   offset += 0.005;
   if (offset > 5) offset = 5;
   r+= 0.008;
   if (r > 50) r = 50;
   theta-= 0.1;
   if (phi <70) phi+= .3;
   if (phi >80) phi-= .3;
   pos.x = r*cos(phi*DEG2RAD)*cos(theta*DEG2RAD);
   pos.y = r*sin(phi*DEG2RAD);
   pos.z = r*cos(phi*DEG2RAD)*cos((90 - theta)*DEG2RAD);

   r /= 3;
   theta -= 60;
   pnt3d tmp = pnt3d();
   tmp.x = r*cos(phi*DEG2RAD)*cos(theta*DEG2RAD);
   tmp.z = r*cos(phi*DEG2RAD)*cos((90 - theta)*DEG2RAD);
   theta += 60;

   pos.px = *px + tmp.x*cine;
   pos.py = *py+offset;
   pos.pz = *pz + tmp.z*cine;
   if ((5-cine)/50 <= .005){ 
   cine+= (5-cine)/50;
   } else {
      cine+=.005;
   }
   if (cine > 1) cine = 3;
}

void cam::zoom(float z){
   pos.x *= z;
   pos.y *= z;
   pos.z *= z;
   //offset *= z;
}

void cam::zoomin(){
   pos.x *= .99;
   pos.y *= .99;
   pos.z *= .99;
   zoomlevel++;
   update(0,0);
}

void cam::zoomout(obj in){
   if(!in.hitCamera(pos.px + pos.x/.99,pos.py + pos.y/.99-.1, pos.pz + pos.z/.99) && zoomlevel > 0){
     pos.x /= .99;
     pos.y /= .99;
     pos.z /= .99;
     zoomlevel--;
     update(0,0);
   }
}


void cam::initPath(){ //set up the intro camera path
  currentPathIndex = 0;
  start = objective_start;
  pathFinished = false;
  intro = true;
  introStart = true;
  if(cam_path.size() > 0){
    finish = *cam_path[0];
  }
  else{
    finish = character_start;
  }
  //cout << "START: " << start.x << " " << start.y << " " << start.z << "\n";
  //cout << "FINISH: " << finish.x << " " << finish.y << " " << finish.z << "\n";  
}

void cam::introScene(pnt3d s, pnt3d f){

  if(introStart){
    pos.px = s.x;
    pos.py = s.y;
    pos.pz = s.z;
    introStart = false;
  }
  
  pnt3d dist;
  
  dist.x = (f.x - pos.px);
  dist.y = (f.y - pos.py); 
  dist.z = (f.z - pos.pz);
  float distance = dist.mag();
  
   if (currentPathIndex+1 < cam_path.size()){
      if (distance < 5){
         float amount = (float)(5 - distance)/5.0;
         dist.x += ((*cam_path[currentPathIndex+1]).x - pos.px)*(amount);
         dist.y += ((*cam_path[currentPathIndex+1]).y - pos.py)*(amount);
         dist.z += ((*cam_path[currentPathIndex+1]).z - pos.pz)*(amount);
      }
   }   

   if(distance < 4){ //continue to next point
      currentPathIndex++;
      //introStart = true;
      if(currentPathIndex < cam_path.size()){
         start = *cam_path[currentPathIndex-1];
         finish = *cam_path[currentPathIndex];
      } else if(currentPathIndex >= cam_path.size()){
         if(!(finish == character_start)){
            start = *cam_path[cam_path.size()-1];
            finish = character_start;
         } else {
            pathFinished = true;
            intro = false;
         }
      }

   }  

  pos.px += 1*dist.x/dist.mag();
  pos.py += 1*dist.y/dist.mag(); 
  pos.pz += 1*dist.z/dist.mag();
  
}

void cam::update(float dx,float dy){
   //cout << "index: " << currentPathIndex << "\n";
   //cout << "size: " << cam_path.size() << "\n";
   /*if(!intro){ //&& currentPathIndex != 100000){
     if(currentPathIndex < cam_path.size()){
       start = *cam_path[currentPathIndex-1];
       finish = *cam_path[currentPathIndex];
       //cout << "first\n";
       intro = true;
       introStart = true;
     }
     else if(currentPathIndex == cam_path.size()){
       if(!pathFinished){
         start = *cam_path[currentPathIndex-1];
         finish = character_start;
         //cout << "second\n";
         //cout << "char_start: " << character_start.x << " " << character_start.y << " " << character_start.z << "\n";
         intro = true;
         introStart = true;
       }
     }
     else{
       pathFinished = true;
       cam_path.clear();
       //currentPathIndex = 100000;
     }
   }
   else { //if intro*/
   if(intro) {
     if (!(dx)&& !(dy)) introScene(start, finish);
   }
   

   // normal camera func.
   if (cinematic) {
      updateCinematic();      
      return;
   }
   theta += 10*(dx); //update view
   phi -= 10*(dy);
   if (phi > 89 || phi < -70){ //limit phi (pitch for view)
      phi += 10*(dy);
   }
   float r;
   r = pos.mag();
   pos.x = r*cos(phi*DEG2RAD)*cos(theta*DEG2RAD);
   pos.y = r*sin(phi*DEG2RAD);
   pos.z = r*cos(phi*DEG2RAD)*cos((90 - theta)*DEG2RAD);
   if(pathFinished){
     pos.px = *px;
     pos.py = *py+offset;
     pos.pz = *pz;
   }
}

void cam::setup(){
   gluLookAt(pos.px+pos.x, pos.py+pos.y, pos.pz+pos.z, //eye position
    pos.px, pos.py, pos.pz, //reference point
    0.0, 1.0, 0.0);  //up vector   
}
