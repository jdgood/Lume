#include "robot.h"
#include "player.h"

extern Player character;
extern char curStage[128];

Robot::Robot()
{
   gameStarted = false;
   x = y = z = 0;
   dir.z = dir.y = 0;
   dir.x = -1;
   stage = 1;
   loadingStage = -1;
   fullLoad = false;
   //   texture.id = 3;
   rando = rand() % 2;
	if (rando == 0) {
      texture.load((char *)"textures/pin_v07_texture.png");
   }
   else {
      texture.load((char *)"textures/moe_v02_Tube.jpg");
   }
   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(1,1,1));
   }
   //model loading
   model();
   if (rando == 0) {
      scale = .2;
      init("models/pin_v06.md5mesh", "models/pin_v06_idle.md5anim");
   }
   else {
      init("models/moe.md5mesh", NULL);
   }
   findBound();
   //model loading done

   maxspeed = MAXSPEED;
   //state = laststate = 0;
   walkacc = 0;
   //nodes = vector<pnt3d>();
   curDest = 0;
   classId = ROB_ID;
   returning = false;

   randomizer = true;

	angle = 0;
   frozen = 0;
}

Robot::Robot(float xp, float yp, float zp)
{
   gameStarted = false;
   x = xp; y = yp; z = zp; by = y;
   dir.z = dir.y = 0;
   dir.x = -1;
   stage = 1;
   loadingStage = -1;
   fullLoad = false;
   rando = rand() % 2;
	if (rando == 0) {
      texture.load((char *)"textures/pin_v07_texture.png");
   }
   else {
      texture.load((char *)"textures/moe_v02_Tube.jpg");
   }
   //  texture.id = 3;
   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(1,1,1));
   }
   //model loading
   model();
   if (rando == 0) {
      scale = .2;
      model::init("models/pin_v06.md5mesh", "models/pin_v06_idle.md5anim");
   }
   else {
      init("models/moe.md5mesh", NULL);
   }
   findBound();
   //model loading done
   float tmp1 = ymax;
   float tmp2 = ymin;   
   ymax = zmax;
   ymin = zmin;
   zmax = tmp1;
   zmin = tmp2;

   maxspeed = MAXSPEED;
      //state = laststate = 0;
   walkacc = 0;
   curDest = 0;
   classId = ROB_ID;
   returning = false;

   randomizer = true;

	angle = 0;
   frozen = 0;
}

Robot::Robot(pnt3d init){
   curDest = 0;
   //nodes = vector<pnt3d>();
   nodes.push_back(init);
   radius = 5.0;
   gameStarted = false;
   x = init.x; y = init.y; z = init.z;
   by = y;
   dir.z = dir.y = 0;
   dir.x = -1;
   stage = 1;
   loadingStage = -1;
   fullLoad = false;
   rando = rand() % 2;
   if (rando == 0) {
      texture.load((char *)"textures/pin_v07_texture.png");
   }
   else {
      texture.load((char *)"textures/moe_v02_Tube.jpg");
   }
   //  texture.id = 10;
   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(1,1,1));
   }
   //model loading
   model();
   if (rando == 0) {
      scale = .2;
      model::init("models/pin_v06.md5mesh", "models/pin_v06_idle.md5anim");

   }
   else {
      model::init("models/moe.md5mesh", NULL);
   }
   findBound();
  /* float tmp1 = ymax;
   float tmp2 = ymin;   
   ymax = zmax;
   ymin = zmin;
   zmax = tmp1;
   zmin = tmp2;*/
   //model loading done
   returning = false;

   randomizer = true;

   maxspeed = MAXSPEED;
      //state = laststate = 0;
   walkacc = 0;
   chase = false;
   angle = rand() % 360;
   classId = ROB_ID;
   frozen = 0;
   //printf("%f %f %f\n\n\n\n\n\n", init.x, init.y, init.z);

}
void Robot::addNode(pnt3d new_node){
   nodes.push_back(new_node);
   //printf("%f %f %f\n\n\n\n\n\n", new_node.x, new_node.y, new_node.z);
}

float Robot::calcVelocityMagn(pnt3d vel) {
   return sqrt(vel.x*vel.x + vel.y*vel.y + vel.z*vel.z);
}

float Robot::distance() {
   return sqrt(pow(character.x - x, 2) + 
         pow(character.y - y, 2) + pow(character.z - z, 2));
}

void Robot::click(float cx, float cy, float cz){
   //printf("Freeze\n");
   character.addEnergy(-1);
   frozen = 200;
   wireFrameBrightness = 1;
}

void Robot::draw(){
   glLoadName(index);
   glPushMatrix();
		// moves robot along sin curve
      glTranslatef(x, y, z); //model.x, etc
      float ds = vel.z/vel.mag();
      float ang = acos(ds)/DEG2RAD;

      if (vel.x < 0) ang = -ang;

      glColor3f(1,1,1);
      //draw bounding box 
      /*glBegin(GL_LINE_LOOP);
         glVertex3f(xmin,ymin,zmin);
         glVertex3f(xmax,ymin,zmin);
         glVertex3f(xmax,ymax,zmin);
         glVertex3f(xmin,ymax,zmin);

         glVertex3f(xmin,ymin,zmax);
         glVertex3f(xmax,ymin,zmax);
         glVertex3f(xmax,ymax,zmax);
         glVertex3f(xmin,ymax,zmax);
      glEnd();*/
      glRotatef(ang,0,1,0);
      //glRotatef(90.0,1,0, 0);
      //glRotatef(180.0,0,1,0);

      if(chase){
         glColor3f(1,0,0);
      }
      if (distance() > 180) {
         ; /* Dont draw */
      }
      else if (distance() > 60) {
         if (rando == 0) {
            //glColor3f(1,1,1);
            Sprite textureLow;
            textureLow.load((char *)"textures/pin.png");
            glBindTexture(GL_TEXTURE_2D, textureLow.id);
            //glutSolidSphere(0.7, 10.0, 10.0);
            GLUquadricObj *mySphere = gluNewQuadric();
            gluQuadricDrawStyle(mySphere, GLU_FILL);
            gluQuadricTexture(mySphere, GL_TRUE);
            gluSphere(mySphere, 0.6, 12, 12);
            glColor3f(0,0,0);
            /*glScalef(10, 0.3, 0.3);
            glutSolidCube(0.3);*/
         }
         else {
            /*glColor3f(0.2,0.2,0.2);
            glScalef(1, 1.5, 1);
            glutSolidCube(0.6);*/
            model::draw();
         }
         //model::draw();
      }
      else {
         if(rando == 0)
            {
               if(chase != oldchase)
                  {
                     if(chase)
                        {
                           model::changeAnimation("models/pin_v06_attack.md5anim");
                           //model::init("models/pin_v06.md5mesh", "models/pin_chase.md5anim");
                           //model::init("models/pin_v06.md5mesh", "models/pin_chase.md5anim");
                        }
                     else
                        {
                           model::changeAnimation("models/pin_v06_idle.md5anim");
                        }
                  }
               if (!frozen){
                  /* Interpolate skeletons between two frames */
                  InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
                                        md5anim.skelFrames[animInfo.next_frame],
                                        md5anim.num_joints,
                                        animInfo.last_time * md5anim.frameRate,
                                        skeleton);
                  
                  /* Calculate current and next frames */
                  Animate (&md5anim, &animInfo, 1);
               } else {
                  if (!models) model::drawWireframe();
               }
            }
         model::draw();
      }
      glPopMatrix();
}

void Robot::simplePath(int direction, float start, float end){
	std::cout << "does something" << std::endl;
  //direction == x
  if(direction == 0){
    if(x <= start){
      pathDirection = false;
    }
    if(x >= end){
      pathDirection = true;
    }
    
    if(pathDirection){
      x -= 0.025;
    }
    else{
      x += 0.025;
    }
  }
  //direction == y
  else if(direction == 1){
    if(y <= start){
      pathDirection = false;
    }
    if(y >= end){
      pathDirection = true;
    }
    
    if(pathDirection){
      y -= 0.025;
    }
    else{
      y += 0.025;
    }
  }
  //direction == z
  else{
    if(z <= start){
      pathDirection = false;
    }
    if(z >= end){
      pathDirection = true;
    }
    
    if(pathDirection){
      z -= 0.025;
    }
    else{
      z += 0.025;
    }
  }

}

void Robot::cleanup() {
   FreeModel (&(md5file));
   FreeAnim (&(md5anim));
   
   if (animated && skeleton)
      {
         free (skeleton);
         skeleton = NULL;
      }

   FreeVertexArrays ();
}

int Robot::checkMove(){
   /*for (unsigned int i = 0; i < objs.size(); i++){//scan all objects
      if (objs[i] != this){
         if (objs[i]->hitCollision(*this)){
            if(objs[i]->classId != MOVINGOBJ_ID && objs[i]->classId != GOBJ_ID)
              return 1;//hit
         }
      }
   }
   */
   return 0;//no problem
}

void Robot::update(float dt){
   if (pnt3d(x, y, z).distBetween(pnt3d(character.x, character.y, 
               character.z)) > 180) {
      return;
   }

   objs = world.getObjs(x,y,z);
   
   float slowthecowdown = .03;
	oldchase = chase;
	
   if(chase){
      //if(20 < abs(x - nodes[curDest].x) || 20 < abs(by - nodes[curDest].y) || 20 < abs(z - nodes[curDest].z)){
      if(20 < abs(x - character.x) || 20 < abs(by - character.y) || 20 < abs(z - character.z)){
         chase = false;
         returning = true;
         vel.x = nodes[curDest].x - x;
         vel.y = nodes[curDest].y - by;
         vel.z = nodes[curDest].z - z;
         vel = vel.normalize(vel);
      }
      else{
         vel.x = character.x - x;
         vel.y = character.y - by;
         vel.z = character.z - z;
         vel = vel.normalize(vel);
         //vel.x *= 2;
         //vel.y *= 2;
         //vel.z *= 2; 
      }
   }
   else if (.1 > abs(x - nodes[curDest].x) && 1 > abs(by - nodes[curDest].y) && .1 > abs(z - nodes[curDest].z)) {
      returning = false;
      curDest++;
      if(curDest == nodes.size()){
         curDest = 0;
      }
      //calculate velocity(x,y,z and nodes[curDest]x,y,z) and normalize
      vel.x = nodes[curDest].x - x;
      vel.y = nodes[curDest].y - by;
      vel.z = nodes[curDest].z - z;
      vel = vel.normalize(vel);
   }
   else if (returning) {
      vel.x = nodes[curDest].x - x;
      vel.y = nodes[curDest].y - by;
      vel.z = nodes[curDest].z - z;
      vel = vel.normalize(vel);
   }
   
   if(!returning && !chase && 5 > abs(x - character.x) && 5 > abs(by - character.y) && 5 > abs(z -character.z)){
      if(strcmp(curStage, "city") && strcmp(curStage, "ghetto")){
         chase = true;
      }
   }
   if (!frozen){
      x += vel.x * dt * slowthecowdown;
      if (!returning && checkMove()){
         x -= vel.x * dt * slowthecowdown;
         curDest++;
         if(curDest == nodes.size()){
            curDest = 0;
         }
         
         if (chase) {
            chase = false;
            oldchase = true;
            returning = true;
         }
      }
     
      by += vel.y * dt * slowthecowdown;
      y = by + sin((float)angle*DEG2RAD);
      if (!returning && checkMove()){
         by -= vel.y * dt * slowthecowdown;
         angle-=5;
         y = by + sin((float)angle*DEG2RAD);
         curDest++;
         if(curDest == nodes.size()){
            curDest = 0;
         }
         
         if (chase) {
            chase = false;
            oldchase = true;
            returning = true;
         }
      }
      
      z += vel.z * dt * slowthecowdown;
      if (!returning && checkMove()){
         z -= vel.z * dt * slowthecowdown;
         curDest++;
         if(curDest == nodes.size()){
            curDest = 0;
         }
         
         if (chase) {
            chase = false;
            oldchase = true;
            returning = true;
         }
      }
      //printf("%f %f %f\n", x, y, z);
      /*std::vector<obj*> objs = world.getObjs(x,y,z);   
      float f = 0;
      if (checkMove(objs)){
         printf("Foobar %d\n",index);      
         y+=0.01;
         //f++;
      }*/
      //y = by + sin((float)angle*DEG2RAD);
      

	// moves robot along sin curve
	   angle++;
	   if(angle > 360) {
		   angle = 0;
	   }
   } else {
      frozen--;
   }

   if (randomizer && !strcmp(curStage, "skyline")) {
      z -= fmod(rand(), (nodes[0].z - nodes[1].z) / 2);
      if (rand()%2 == 0) { 
         curDest++;
         vel.z *= -1;
         if(curDest == nodes.size()){
            curDest = 0;
         }
      }
      randomizer = false;
   }

   if (hitCollision(character)){
      pnt3d fb = pnt3d(character.x - x, 0, character.z - z);
      fb = fb.normalize(fb);
      character.vel.x = fb.x*2;
      character.vel.z = fb.z*2;
      grav = -.3;
      fbo->smallflash();
      sound.play("enemy.wav",x,y,z);
   }
}

float Robot::distance(obj* o){
   float a = o->x- x;
   float b = o->y - y;
   float c = o->z - z;
   a = sqrt(a*a + b*b + c*c);
   return a;
}

