#include "tree.h"

Tree::Tree() {
   classId = TREE_ID;
}

Tree::Tree(float xp, float yp, float zp) {
	x = xp;
	y = yp;
	z = zp;
   classId = TREE_ID;
   xmax = 3.0;//bounds
   ymax = 9.0;
   zmax = 3.0;
   xmin = -3;//bounds
   ymin = 0;
   zmin = 3.0;
   float tMax = max(abs((xmax) - (xmin)), abs((ymax) - (ymin)));
   radius = max(abs(tMax), abs((zmax) - (zmin)));
	growing = false;

	model::init("models/tree.md5mesh", "models/tree.md5anim");

	texture.load((char *)"textures/treetexturedead.png");
	for (int i = 0; i < 8; i++){
	   colors.push_back(pnt3d(1,1,1));
	}
}

bool Tree::hitCollision(obj object){
   return false;
}

void Tree::draw() {
   glPushMatrix();
   glTranslatef(x, y, z);
   //glScalef(0.75, 0.75, 0.75);
   
   if(growing)
      {
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
   else
      {
         if(animInfo.next_frame > animInfo.curr_frame) {
            /* Interpolate skeletons between two frames */
            InterpolateSkeletons (md5anim.skelFrames[1],
                                  md5anim.skelFrames[2],
                                  md5anim.num_joints,
                                  animInfo.last_time * md5anim.frameRate,
                                  skeleton);
         }
         
         if(pnt3d(x, y, z).distBetween(pnt3d(character.x, character.y, character.z)) < 5) {
            growing = true;
            texture.load((char *)"textures/treetexture.png");
         }
      }
   
   // draws model
   glPushMatrix();
   glRotatef(180.0, 0.0, 1.0, 0.0);
   if( pnt3d(x, y, z).distBetween(pnt3d(character.x, character.y, character.z)) < 60 )
      model::draw();

   glPopMatrix();
   glPopMatrix();

   //BOUNDING BOX
   /*glPushMatrix();
      glBegin(GL_LINE_LOOP);
      glVertex3f(xmax, ymax, zmax);
      glVertex3f(xmax, ymax, zmin);
      glVertex3f(xmax, ymin, zmax);
      glVertex3f(xmax, ymin, zmin);
      glVertex3f(xmin, ymax, zmax);
      glVertex3f(xmin, ymax, zmin);
      glVertex3f(xmin, ymin, zmax);
      glVertex3f(xmin, ymin, zmin);
      glEnd();
   glPopMatrix();*/
}


