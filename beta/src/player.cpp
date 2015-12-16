#include "player.h"

Player::Player()
{
   gameStarted = false;
   x = y = z = 0;
   dir.z = dir.y = 0;
   dir.x = -1;
   curenergy=0;
   sprintEnergy=0;
   frameCount = 0;
   isBuilding = false;
   isRemoving = false;
   canSprint = true;
   exp = 0;
   setLevel(1);
   stage = 1;
   loadingStage = -1;
   fullLoad = false;
   setLevel(1);
   checkpoint.x = checkpoint.y = checkpoint.z = 0;
   texture.id = 2;
   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(1,1,1));
   }
   //model loading
   model();
   scale = .2;
   init("models/lume2.md5mesh", "models/guy_breath.md5anim");
   findBound();
   //model loading done
   target = NULL;

   maxspeed = MAXSPEED;
   maxjump = MAXJUMP;
   isJumping = false;
   isSprinting = false;
   state = laststate = 0;
   walkacc = 0;
}

Player::Player(float xp, float yp, float zp)
{
   gameStarted = false;
   x = xp; y = yp; z = zp;
   dir.z = dir.y = 0;
   dir.x = -1;
   curenergy=0;
   sprintEnergy=0;
   frameCount = 0;
   isBuilding = false;
   isRemoving = false;
   canSprint = true;
   exp = 0;
   setLevel(1);
   stage = 1;
   loadingStage = -1;
   fullLoad = false;
   setLevel(1);
   checkpoint.x = checkpoint.y = checkpoint.z=0;
   texture.load((char *)"textures/bill.jpg");
   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(1,1,1));
   }
      //charModel = model();
   model();
   scale = .2;
   init("models/lume2.md5mesh", "models/guy_breath.md5anim");
   findBound();

   maxspeed = MAXSPEED;
   maxjump = MAXJUMP;
   isJumping = false;
   isSprinting = false;
   walkacc = 0;
}

Player::Player(float xp, float yp, float zp, int curenrgy, int totalenrgy, float maxspeed, float maxjump)
 : curenergy(curenrgy), totalenergy(totalenrgy), maxspeed(maxspeed), maxjump(maxjump)
{
   gameStarted = false;
   //xmin = -0.5; ymin = 0.5; zmin = -0.5; xmax = 0.5; ymax = 1.5; zmax = 0.5;
   dir.z = dir.y = 0;
   dir.x = -1;
   curenergy=0;
   sprintEnergy=0;
   exp = 0;
   setLevel(1);
   stage = 1;
   loadingStage = -1;
   fullLoad = false;
   texture.load((char *)"textures/bill.jpg");
   for (int i = 0; i < 8; i++){
      colors.push_back(pnt3d(1,1,1));
   }
   x = xp; y = yp; z = zp;
      //charModel = model();
   model();
   scale = .2;
   init("models/lume2.md5mesh", "models/guy_breath.md5anim");
   findBound();

   walkacc = 0;
}

float Player::calcVelocityMagn(pnt3d vel) {
   return sqrt(vel.x*vel.x + vel.y*vel.y + vel.z*vel.z);
}

void Player::draw(){
   //bloom->bind();
   glPushMatrix();
      glTranslatef(x, y, z); //model.x, etc
      float ds = dir.z/dir.mag();
      float ang = acos(ds)/DEG2RAD;

      if (dir.x < 0) ang = -ang;

      glRotatef(ang,0,1,0);

      // draw bounding box
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

        if (state == 0 && state != laststate) { // still
           animated = 0;
           laststate = state;
           model::changeAnimation("models/guy_breath.md5anim");
        }
        else if (state == 1 && state != laststate) { // walk
           //printf("in walk statement!!\n");
           animated = 1;
           laststate = state;
           model::changeAnimation("models/guy_walk.md5anim");
        }
        else if (state == 2 && state != laststate) { // jump
           animated = 1;
           laststate = state;
           model:: changeAnimation("models/guy_newjump.md5anim");
        }
        else if (state == 3 && state != laststate) { // build block
           //printf("in build statement!!\n");
           animated = 1;
           laststate = state;
           model:: changeAnimation("models/guy_build_out.md5anim");
        }
        else if (state == 4 && state != laststate) { // remove block
           //printf("in remove statement!!\n");
           animated = 1;
           laststate = state;
           model:: changeAnimation("models/guy_build_in.md5anim");
        }

        if(state == 0)
		  {
				model::drawNormal();
		  }
		  else
		  {
        		model::draw(calcVelocityMagn(vel));
        }
		  glPopMatrix();
        //bloom->unbind();
}

void Player::cleanup() {
   FreeModel (&(md5file));
   FreeAnim (&(md5anim));
   
   if (animated && skeleton)
      {
         free (skeleton);
         skeleton = NULL;
      }

   FreeVertexArrays ();
}

void Player::setCheckpoint(float xcheck, float ycheck, float zcheck){
  checkpoint.x = xcheck;
  checkpoint.y = ycheck;
  checkpoint.z = zcheck;
}

void Player::setLevel(int l){
   //world.add(new obj2d(x,y,z,&(camera.pos)));
   //if (lvlFunc) (*lvlFunc)();
   level = l;
   expNeeded = level*10;
   totalenergy = level*5;
   buildlvl = (int)level/5 + 1;
   maxspeed = MAXSPEED + (level * 0.0010);
   maxjump = MAXJUMP + (level * 0.003);
}

void Player::addExp(float e){
   exp+= e;
   if (exp < 0) exp = 0;
   while (exp >= expNeeded){
      exp -= expNeeded;
      setLevel(level+1);
   }
}

void Player::addEnergy(int e){
   curenergy += e;
   if (curenergy < 0) curenergy = 0;
   if (curenergy > totalenergy) curenergy = totalenergy;
}

void Player::setEnergy(int en){
  
  if(en < 0){
    en = 0;
  }
  if(en > totalenergy){
    curenergy = totalenergy;
    //totalenergy = en;
  } else {
    curenergy = en;
  }
  
  maxspeed = MAXSPEED + (totalenergy * 0.0002);
  maxjump = MAXJUMP + (totalenergy/3.0 * 0.001);
}

void Player::setTotalEnergy(int en){
  if(en < 0){
    en = 0;
  }
  curenergy = en;
  totalenergy = en;
  maxspeed = MAXSPEED + (totalenergy * 0.0002);
  maxjump = MAXJUMP + (totalenergy * 0.001);
}

int Player::checkMove(std::vector<obj*> objs){
   ymin+= .4;//offset for walking up slopes
   for (unsigned int i = 0; i < objs.size(); i++){//scan all objects
    if (objs[i]->hitCollision(*this)){//*objs[i])){ 
      if (objs[i]->classId == ROB_ID){
         ymin-= .4;
         return -1;
      }    
      ymin-= .4;
      return 1;
    }
   }
   ymin-=.4;
   return 0;
}

int Player::checkTarget(std::vector<obj*> objs){
   if (!target) return 0;
   for (unsigned int i = 0; i < objs.size(); i++){//scan all objects
      if (objs[i] != target){
         if (objs[i]->hitCollision(*target)){   
            return 1;//hit
         }
      }
   }
   return 0;//no problem
}

void Player::update(float dt, std::vector<obj*> objs){
   /*if (!canSprint && sprintEnergy >= 50) {
      sprintEnergy += dt;
      if (sprintEnergy >= 250) {
         sprintEnergy = 0;
         canSprint = true;
      }
   }
   else if (isSprinting) {
      sprintEnergy += dt;
      if (sprintEnergy >= 50) {
         canSprint = false;
      }
   }
   else {
      sprintEnergy -= dt/3;
      if (sprintEnergy <= 0) {
         sprintEnergy = 0;
      }
   }*/
   if (isSprinting) {
      sprintEnergy += dt;
      if (sprintEnergy >= 50) {
         canSprint = false;
      }
   }
   int tst;
   //if (!editmode) world.add(newTrail(x,y,z,&(camera.pos)));//trail

   if (!target) {
      //player movement
      //try x movement
      x += vel.x * dt;
      if ((tst = checkMove(objs))){
        if (tst < 0) return;
        x -= vel.x * dt;
      }
      //try z movement
      z += vel.z * dt;
      if ((tst = checkMove(objs))){
        if (tst < 0) return;
        z -= vel.z * dt;
      }
      // light up blur outlines when close to player
      for (unsigned i = 0; i < objs.size(); i++){
         if ((objs[i]->classId != MOVINGOBJ_ID) && y + 20 > objs[i]-> y && y - 20 < objs[i]-> y ) objs[i]->wireFrameBrightness = 1;
      }
   } else {
      objs = world.getObjs(target->x,target->y,target->z);
      target->x += vel.x * dt;
      if ((checkTarget(objs))){
        target->x -= vel.x * dt;
        vel.x = 0;
      }
      //try z movement
      target->z += vel.z * dt;
      if ((checkTarget(objs))){
        target->z -= vel.z * dt;
        vel.z = 0;
      }   
   }

   if (frameCount >= 0 && frameCount < 25) {
      // printf("frameCount = %d\n", frameCount);
      frameCount++;
   }
   else if (frameCount >= 25) {
      // printf("frameCount = %d\n", frameCount);
      frameCount = 0;
      if (isBuilding == true)
         isBuilding = false;
      if (isRemoving == true)
         isRemoving = false;
   }
   // printf("frameCount = %d\n", frameCount);
   //printf("isBuilding = %d\n", isBuilding);
   // printf("isRemoving = %d\n", isRemoving);

   // Set animation state
   // note: logic makes no sense to me for the jump and walk, but it works...

   if (isBuilding && ((int)(100*calcVelocityMagn(vel)) == 0)) {
      state = 3;
			if(animInfo.next_frame > animInfo.curr_frame) {
				/* Interpolate skeletons between two frames */
				InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
				                      md5anim.skelFrames[animInfo.next_frame],
				                      md5anim.num_joints,
				                      animInfo.last_time * md5anim.frameRate,
				                      skeleton);
				
				/* Calculate current and next frames */
				Animate (&md5anim, &animInfo, 1);
		   }
   }
   else if (isRemoving && ((int)(100*calcVelocityMagn(vel)) == 0)) {
      state = 4;
			if(animInfo.next_frame > animInfo.curr_frame) {
				/* Interpolate skeletons between two frames */
				InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
				                      md5anim.skelFrames[animInfo.next_frame],
				                      md5anim.num_joints,
				                      animInfo.last_time * md5anim.frameRate,
				                      skeleton);
				
				/* Calculate current and next frames */
				Animate (&md5anim, &animInfo, 1);
		   }
   }
   else if ((int)(100*calcVelocityMagn(vel)) == 0) { // still
      state = 0;
		//printf("idle now\n");
   }
   else if ((int)(100*calcVelocityMagn(vel)) > 0 && !jumpActive) { // jump
      state = 2;
   }
   else if (jumpActive) { // walk
      state = (!target);
   }

   // printf("state = %d\n", state);
}

//void Player::addListener(void (*lvlF)()){
//   lvlFunc = lvlF;
//}

float Player::distance(obj* o){
   float a = o->x- x;
   float b = o->y - y;
   float c = o->z - z;
   a = sqrt(a*a + b*b + c*c);
   return a;
}

void Player::control(obj *o){
   addEnergy(-1);
   target = o;
   camera.lookAt(&(o->x),&(o->y),&(o->z));
}

void Player::releaseControl(){
   target->click(0,0,0);
   target = NULL;
   camera.lookAt(&(x),&(y),&(z));
}

void Player::respawn(){
     x = checkpoint.x;
     y = checkpoint.y + 1;
     z = checkpoint.z;
     fbo->flash();
}

void Player::setVel(float dx, float dz){
  if (fbo->state == 2){
      vel.x *= .7;
      vel.z *= .7;
      return;
  }
  if (walkacc < 1) walkacc += .1;
  if (walkacc > 1) walkacc = 1;
  float xyMag = sqrt(camera.pos.x * camera.pos.x + camera.pos.z * camera.pos.z);
  vel.x = camera.pos.x/xyMag * dz * walkacc - camera.pos.z/xyMag * dx * walkacc;
  vel.z = camera.pos.z/xyMag * dz * walkacc + camera.pos.x/xyMag * dx * walkacc;
  dir.x = vel.x;
  dir.z = vel.z;
}

