#include "frustum.h"

bool frustum::PointInFrustum(pnt3d p){
  int i;
  for(i = 0; i < 6; i++)
  {
    if(planes[i].a * p.x + planes[i].b * p.y + planes[i].c * p.z + planes[i].d <= 0){
	   return(false);
	 }
  }
  return(true);
}

bool frustum::PointInFrustumV(pnt3d p){//without top and bottom planes
  int i;
  for(i = 0; i < 6; i++)
  {
    if (i != 2 && i != 3){
      if(planes[i].a * p.x + planes[i].b * p.y + planes[i].c * p.z + planes[i].d <= 0){
	      return(false);
	   }
    }
  }
  return(true);
}

bool frustum::radiusInFrustum(pnt3d p, float radius){
  for(int i = 0; i < 6; i++){
    float signeddistance = (planes[i].a * p.x + planes[i].b * p.y + planes[i].c * p.z + planes[i].d) 
                           / (sqrt(planes[i].a*planes[i].a + planes[i].b*planes[i].b + planes[i].c*planes[i].c));
    if(signeddistance < -radius){
      return false;
    }
  }
  return true;
}

bool frustum::radiusInFrustumV(pnt3d p, float radius){//without top and bottom planes
  for(int i = 0; i < 6; i++){
    if (i != 2 && i != 3){
       float signeddistance = (planes[i].a * p.x + planes[i].b * p.y + planes[i].c * p.z + planes[i].d) 
                              / (sqrt(planes[i].a*planes[i].a + planes[i].b*planes[i].b + planes[i].c*planes[i].c));
       if(signeddistance < -radius){
         return false;
       }
    }
  }
  return true;
}

bool frustum::needsCull(obj* object){
  
  //pnt3d point[8];
  //float px = object->x - camera.pos.px - camera.pos.x;
    //float pz = object->z - camera.pos.pz - camera.pos.z;
    //if ((camera.pos.x*px < 0) && (camera.pos.z*pz < 0)) return true;
  /*point[0].x = object->xmin;
  point[0].y = object->ymin;
  point[0].z = object->zmin;
  
  point[1].x = object->xmax;
  point[1].y = object->ymin;
  point[1].z = object->zmin;
  
  point[2].x = object->xmin;
  point[2].y = object->ymax;
  point[2].z = object->zmin;
  
  point[3].x = object->xmax;
  point[3].y = object->ymax;
  point[3].z = object->zmin;
  
  point[4].x = object->xmin;
  point[4].y = object->ymin;
  point[4].z = object->zmax;
  
  point[5].x = object->xmax;
  point[5].y = object->ymin;
  point[5].z = object->zmax;
  
  point[6].x = object->xmin;
  point[6].y = object->ymax;
  point[6].z = object->zmax;
  
  point[7].x = object->xmax;
  point[7].y = object->ymax;
  point[7].z = object->zmax;*/
  /*point[0].x = object->xmin + object->x;
  point[0].y = object->ymin + object->y;
  point[0].z = object->zmin + object->z;
  
  point[1].x = object->xmax + object->x;
  point[1].y = object->ymin + object->y;
  point[1].z = object->zmin + object->z;
  
  point[2].x = object->xmin + object->x;
  point[2].y = object->ymax + object->y;
  point[2].z = object->zmin + object->z;
  
  point[3].x = object->xmax + object->x;
  point[3].y = object->ymax + object->y;
  point[3].z = object->zmin + object->z;
  
  point[4].x = object->xmin + object->x;
  point[4].y = object->ymin + object->y;
  point[4].z = object->zmax + object->z;
  
  point[5].x = object->xmax + object->x;
  point[5].y = object->ymin + object->y;
  point[5].z = object->zmax + object->z;
  
  point[6].x = object->xmin + object->x;
  point[6].y = object->ymax + object->y;
  point[6].z = object->zmax + object->z;
  
  point[7].x = object->xmax + object->x;
  point[7].y = object->ymax + object->y;
  point[7].z = object->zmax + object->z;*/
  
  /*for(int k = 0; k < 8; k++){
    if(PointInFrustum(point[k])){
	   return false;
	 }
  }*/

  pnt3d objcenter = pnt3d(object->x, object->y, object->z);
  
  if(radiusInFrustum(objcenter, object->radius)){
    return false;
  }
  
  return true;
}

bool frustum::shadowNeedsCull(obj* object){
  
  pnt3d point[8];
  
  point[0].x = object->xmin;
  point[0].y = object->ymin;
  point[0].z = object->zmin;
  
  point[1].x = object->xmax;
  point[1].y = object->ymin;
  point[1].z = object->zmin;
  
  point[2].x = object->xmin;
  point[2].y = object->ymax;
  point[2].z = object->zmin;
  
  point[3].x = object->xmax;
  point[3].y = object->ymax;
  point[3].z = object->zmin;
  
  point[4].x = object->xmin;
  point[4].y = object->ymin;
  point[4].z = object->zmax;
  
  point[5].x = object->xmax;
  point[5].y = object->ymin;
  point[5].z = object->zmax;
  
  point[6].x = object->xmin;
  point[6].y = object->ymax;
  point[6].z = object->zmax;
  
  point[7].x = object->xmax;
  point[7].y = object->ymax;
  point[7].z = object->zmax;
  
  for(int k = 0; k < 8; k++){
    if(PointInFrustumV(point[k])){
	   return false;
	 }
  }
  
  pnt3d objcenter = pnt3d(object->x, object->y, object->z);
  
  if(radiusInFrustumV(objcenter, object->radius)){
    return false;
  }
  
  return true;
}

bool frustum::needsCull(obj2d* object){
  pnt3d objcenter = pnt3d(object->x, object->y, object->z);
  
  if(radiusInFrustum(objcenter, object->radius)){
    return false;
  }
  
  return true;
}

void frustum::updateMatrix(cam camera){
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();  
  glLoadIdentity();
  camera.setup();
  glGetFloatv(GL_MODELVIEW_MATRIX , m);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix(); 
  glMultMatrixf(m);
  glGetFloatv(GL_PROJECTION_MATRIX , m);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  
  GLfloat	t;
  
  // Extract The Numbers For The RIGHT Plane
  planes[0].a = m[ 3] - m[ 0];
  planes[0].b = m[ 7] - m[ 4];
  planes[0].c = m[11] - m[ 8];
  planes[0].d = m[15] - m[12];

  // Normalize The Result
  t = GLfloat(sqrt( planes[0].a * planes[0].a + planes[0].b * planes[0].b + planes[0].c * planes[0].c ));
  planes[0].a /= t;
  planes[0].b /= t;
  planes[0].c /= t;
  planes[0].d /= t;

  // Extract The Numbers For The LEFT Plane
  planes[1].a = m[ 3] + m[ 0];
  planes[1].b = m[ 7] + m[ 4];
  planes[1].c = m[11] + m[ 8];
  planes[1].d = m[15] + m[12]; 
  // Normalize The Result
  t = GLfloat(sqrt( planes[1].a * planes[1].a + planes[1].b * planes[1].b + planes[1].c * planes[1].c ));
  planes[1].a /= t;
  planes[1].b /= t;
  planes[1].c /= t;
  planes[1].d /= t; 
  // Extract The BOTTOM Plane
  planes[2].a = m[ 3] + m[ 1];
  planes[2].b = m[ 7] + m[ 5];
  planes[2].c = m[11] + m[ 9];
  planes[2].d = m[15] + m[13];

  // Normalize The Result
  t = GLfloat(sqrt( planes[2].a * planes[2].a + planes[2].b * planes[2].b + planes[2].c * planes[2].c ));
  planes[2].a /= t;
  planes[2].b /= t;
  planes[2].c /= t;
  planes[2].d /= t;

  // Extract The TOP Plane
  planes[3].a = m[ 3] - m[ 1];
  planes[3].b = m[ 7] - m[ 5];
  planes[3].c = m[11] - m[ 9];
  planes[3].d = m[15] - m[13];

  // Normalize The Result
  t = GLfloat(sqrt( planes[3].a * planes[3].a + planes[3].b * planes[3].b + planes[3].c * planes[3].c ));
  planes[3].a /= t;
  planes[3].b /= t;
  planes[3].c /= t;
  planes[3].d /= t;
  // Extract The FAR Plane
  planes[4].a = m[ 3] - m[ 2];
  planes[4].b = m[ 7] - m[ 6];
  planes[4].c = m[11] - m[10];
  planes[4].d = m[15] - m[14];

  // Normalize The Result
  t = GLfloat(sqrt( planes[4].a * planes[4].a + planes[4].b * planes[4].b + planes[4].c * planes[4].c ));
  planes[4].a /= t;
  planes[4].b /= t;
  planes[4].c /= t;
  planes[4].d /= t;

  // Extract The NEAR Plane
  planes[5].a = m[ 3] + m[ 2];
  planes[5].b = m[ 7] + m[ 6];
  planes[5].c = m[11] + m[10];
  planes[5].d = m[15] + m[14];

  // Normalize The Result
  t = GLfloat(sqrt( planes[5].a * planes[5].a + planes[5].b * planes[5].b + planes[5].c * planes[5].c ));
  planes[5].a /= t;
  planes[5].b /= t;
  planes[5].c /= t;
  planes[5].d /= t;  
}

pnt3d frustum::canonic(float x, float y, float z) {
   pnt3d p;
   p.x = x * m[0];
   p.x += y * m[4];
   p.x += z * m[8];
   p.x += m[12];

   p.y = x * m[1];
   p.y += y * m[1 + 4];
   p.y += z * m[1 + 8];
   p.y += m[1 + 12];

   p.z = x * m[2];
   p.z += y * m[2 + 4];
   p.z += z * m[2 + 8];
   p.z += m[2 + 12];

   float w;
   w = x * m[3];
   w += y * m[3 + 4];
   w += z * m[3 + 8];
   w += m[3 + 12];

   p.x /= w;
   p.y /= w;
   p.z /= w;
   return p;
}

void frustum::printm(){
  printf("[%f][%f][%f][%f]\n", m[0], m[1], m[2], m[3]);
  printf("[%f][%f][%f][%f]\n", m[4], m[5], m[6], m[7]);
  printf("[%f][%f][%f][%f]\n", m[8], m[9], m[10], m[11]);
  printf("[%f][%f][%f][%f]\n", m[12], m[13], m[14], m[15]);
}
