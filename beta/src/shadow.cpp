#include "shadow.h"

float dot_product(float v1[3], float v2[3])
{
	return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

void normalize(float v[3])
{
	float f = 1.0f / sqrt(dot_product(v, v));
	v[0] *= f;
	v[1] *= f;
	v[2] *= f;
}

void shadowPlane()
{
   glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//modelview
	   glLoadIdentity();
	   glMatrixMode(GL_PROJECTION);
	   glPushMatrix();
	      glLoadIdentity();
	      glOrtho(0, 1, 0, 1, 0, 1);
	      glDisable(GL_DEPTH_TEST);

	      glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
	      glBegin(GL_QUADS);
		      glVertex2f(0, 0);
		      glVertex2f(0, 1);
		      glVertex2f(1, 1);
		      glVertex2f(1, 0);
	      glEnd();

	      glEnable(GL_DEPTH_TEST);
	   glPopMatrix();
	   glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void shadow::make(obj* o, int index)//, surf s, float lx, float ly, float lz)//makes shadow volumes
{
	float v[4][3];
   float a[4][3];
	for (unsigned int i = 0; i < 4; i++){
      a[i][0] = (o->surface[index].verts[i].x + o->x);//real vert locations
		a[i][1] = (o->surface[index].verts[i].y + o->y);
		a[i][2] = (o->surface[index].verts[i].z + o->z);

		v[i][0] = 0;//(o->surface[index].verts[i].x + o->x - light.x);//distance between verts and light (light to vert vector)
		v[i][1] = -1;//(o->surface[index].verts[i].y + o->y - light.y);//-1
		v[i][2] = 0;//(o->surface[index].verts[i].z + o->z - light.z);
		normalize(v[i]);
		v[i][0] *= M_INFINITY;//make very long
		v[i][1] *= M_INFINITY;
		v[i][2] *= M_INFINITY;
		v[i][0] += o->x;//set back to surface pos
		v[i][1] += o->y;
		v[i][2] += o->z;
	}

	/* back cap */
	glBegin(GL_QUADS);
		glVertex3fv(v[3]);
		glVertex3fv(v[2]);
		glVertex3fv(v[1]);
		glVertex3fv(v[0]);
	glEnd();

	/* front cap */
	glBegin(GL_QUADS);
		glVertex3fv(a[0]);
		glVertex3fv(a[1]);
		glVertex3fv(a[2]);
		glVertex3fv(a[3]);
	glEnd();

   //sides
	glBegin(GL_QUAD_STRIP);

	for(int i = 0; i <= 4; i++) {
		glVertex3fv(a[i%4]);
		glVertex3fv(v[i%4]);
	}

	glEnd();
}

void shadow::draw(obj* o)//float lx, float ly, float lz)
{	
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable (GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);

      //glClearStencil(1);
      //glClear(GL_STENCIL_BUFFER_BIT);
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, -1);
      //glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
      for (unsigned int i = 0; i < o->surface.size(); i++){
      glColor4f(0.0f, 0.0f, 0.0f, 0.01f);
      glEnable(GL_STENCIL_TEST);
         glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		      glDepthMask(GL_FALSE);
		         glEnable(GL_CULL_FACE);

		            glEnable(GL_POLYGON_OFFSET_FILL);//makes so shadow not on top of surface
		            glPolygonOffset(0.0f, 400.0f);
                     
		               glCullFace(GL_FRONT);
                     //glFrontFace( GL_CCW );		               
                     glStencilFunc(GL_ALWAYS, 0, 0xff);
		               glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
                     //glStencilOp(GL_KEEP, GL_KEEP,GL_INCR);
		               make(o, i);//keep back facing shadow
		               glCullFace(GL_BACK);
	                  //glFrontFace( GL_CW );
		               glStencilFunc(GL_ALWAYS, 0, 0xff);
                     glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		               //glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		               make(o, i);//mask out front facing shadow

		            glDisable(GL_POLYGON_OFFSET_FILL);
		         glDisable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);
		   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		   
		   glStencilFunc(GL_NOTEQUAL, 0, 0xff);
		   glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
         //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		   shadowPlane();//draw actuall shadow
   glDisable(GL_BLEND);
		   glDisable(GL_STENCIL_TEST);
      }


      glPopMatrix();
}
