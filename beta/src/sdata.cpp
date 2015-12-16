#include "sdata.h"

SData::SData(){
   xscale = 20;
   zscale = 20;
   xoffset = -10;
   zoffset = -10;
   for (int i = 0; i < SD_LENGTH; i++){
      for (int j = 0; j < SD_WIDTH; j++){
         std::vector<obj*> r;
         objs[i][j] = r;
         offsets[i][j] = 0.001;
      }
   }
}

void SData::resize(float offsetx, float offsetz, float lengthx, float lengthz){
   //printf("resized: %f %f %f %f\n",lengthx,lengthz,offsetx,offsetz);
   xscale = (int)lengthx/SD_LENGTH + 1;
   zscale = (int)lengthz/SD_WIDTH + 1;
   if (xscale < 10) xscale = 10;
   if (zscale < 10) zscale = 10;
   xoffset = (int)offsetx;
   zoffset = (int)offsetz;
   //printf("resized: %d %d %d %d\n",xscale,zscale,xoffset,zoffset);
}

void SData::removeAll(){
   for (int i = 0; i < SD_LENGTH; i++){
      for (int j = 0; j < SD_WIDTH; j++){
         objs[i][j].clear();
         offsets[i][j] = 0.001;
      }
   }
   all.clear();
   all2d.clear();
   sobj.clear();
   robs.clear();
   floaters.clear();
}

obj* SData::get(int index){
   if (index < 0 || index >= (int)all.size()) return NULL;
   return (all[index]);
}

void SData::remove(int index){
   if (index < 0 || index >= (int)all.size()) return;
   int i,j,k;

   for (unsigned int b = 0; b < all[index]->buckets.size(); b++){
      i = all[index]->buckets[b].x;
      j = all[index]->buckets[b].y;
      k = all[index]->buckets[b].z;
      std::vector<obj*> bucket = (objs[i][j]);
      for (int a = k + 1; a < (int)bucket.size(); a++){
         for (unsigned int c = 0; c < bucket[a]->buckets.size(); c++){
            if (bucket[a]->buckets[c].x == i && bucket[a]->buckets[c].y == j){
               bucket[a]->buckets[c].z-=1;
            }
         }
      }
      bucket.erase(bucket.begin()+k);
      objs[i][j] = bucket;
   }

   for (unsigned int a = index + 1; a < all.size(); a++){
      all[a]->index-=1;
   }
   all.erase(all.begin()+index);

   for (unsigned int a = 0; a < sobj.size(); a++){
      if (sobj[a]->index == index){
         sobj.erase(sobj.begin()+a);
         return;
      }
   }
}

void SData::remove(obj* o){
   int i,j,k;
   for (unsigned int b = 0; b < o->buckets.size(); b++){
      i = o->buckets[b].x;
      j = o->buckets[b].y;
      k = o->buckets[b].z;
      std::vector<obj*> bucket = (objs[i][j]);
      for (int a = k + 1; a < (int)bucket.size(); a++){
         for (unsigned int c = 0; c < bucket[a]->buckets.size(); c++){
            if (bucket[a]->buckets[c].x == i && bucket[a]->buckets[c].y == j){
               bucket[a]->buckets[c].z-=1;
            }
         }
      }
      bucket.erase(bucket.begin()+k);
      objs[i][j] = bucket;
   }
   o->buckets.clear();

   for (unsigned int a = o->index + 1; a < all.size(); a++){
      all[a]->index-=1;
   }
   all.erase(all.begin()+o->index);

   for (unsigned int a = 0; a < sobj.size(); a++){
      if (sobj[a]->index == o->index){
         sobj.erase(sobj.begin()+a);
         return;
      }
   }
}

void SData::remove2d(int index){
   for (unsigned int a = index + 1; a < all2d.size(); a++){
      all2d[a]->index-=1;
   }
   all2d.erase(all2d.begin()+index);
}

bool SData::inBucket(int i, int j, obj* o){
  if((xoffset + xscale*(i+1)) <= (o->x + o->xmin)){
    return false;
  }
  else if((xoffset + xscale*i) >= (o->x + o->xmax)){
    return false;
  }
  else if((zoffset + zscale*(j+1)) <= (o->z + o->zmin)){
    return false;
  }
  else if((zoffset + zscale*j) >= (o->z + o->zmax)){
    return false;
  }
  return true;
}

void SData::add(obj* o){
   int i = (o->x + o->xmin - xoffset)/xscale;
   int j = (o->z + o->zmin - zoffset)/zscale;
   if (i < 0) i = 0;
   if (j < 0) j = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;
   //starts from bottom left corner
   while (j < SD_WIDTH && inBucket(i,j,o)){
      int ti = i;
      while (ti < SD_LENGTH && inBucket(ti,j,o)){
         objs[ti][j].push_back(o);
         o->buckets.push_back(int3d(ti,j,objs[ti][j].size() -1));
         ti++;
      }
      j++;
   }

   all.push_back(o);
   o->index = all.size() -1;
}

void SData::update(int index){
   obj * o = all[index];
   int i,j,k;
   for (unsigned int b = 0; b < o->buckets.size(); b++){
      i = o->buckets[b].x;
      j = o->buckets[b].y;
      k = o->buckets[b].z;
      std::vector<obj*> bucket = (objs[i][j]);
      for (int a = k + 1; a < (int)bucket.size(); a++){
         for (unsigned int c = 0; c < bucket[a]->buckets.size(); c++){
            if (bucket[a]->buckets[c].x == i && bucket[a]->buckets[c].y == j){
               bucket[a]->buckets[c].z-=1;
            }
         }
      }
      bucket.erase(bucket.begin()+k);
      objs[i][j] = bucket;
   }
   o->buckets.clear();

   for (unsigned int a = 0; a < sobj.size(); a++){
      if (sobj[a]->index == o->index){
         sobj.erase(sobj.begin()+a);
         return;
      }
   }

   i = (o->x + o->xmin - xoffset)/xscale;
   j = (o->z + o->zmin - zoffset)/zscale;
   if (i < 0) i = 0;
   if (j < 0) j = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;
   //starts from bottom left corner
   while (j < SD_WIDTH && inBucket(i,j,o)){
      int ti = i;
      while (ti < SD_LENGTH && inBucket(ti,j,o)){
         objs[ti][j].push_back(o);
         o->buckets.push_back(int3d(ti,j,objs[ti][j].size() -1));
         ti++;
      }
      j++;
   }
}

void SData::addRobot(obj* o){
   robs.push_back(o);
   //o->robotId = robs.size() -1;

   all.push_back(o);
   o->index = all.size() -1;
}

//UPDATE THIS TO STORE MOVING OBJECTS SOMEWHERE OTHER THAN ROBS
void SData::addMovingObj(obj* o){
   robs.push_back(o);
   //o->robotId = robs.size() -1;

   all.push_back(o);
   o->index = all.size() -1;
}

void SData::addShadow(obj* o){
   sobj.push_back(o);
}

bool SData::inBucket(int i, int j, obj2d* o){
  if((xoffset + xscale*(i+1)) < (o->x - o->radius)){
    return false;
  }
  else if((xoffset + xscale*i) > (o->x + o->radius)){
    return false;
  }
  else if((zoffset + zscale*(j+1)) < (o->z - o->radius)){
    return false;
  }
  else if((zoffset + zscale*j) > (o->z + o->radius)){
    return false;
  }
  return true;
}


void SData::add(obj2d* o){
   int i = (o->x - o->radius - xoffset)/xscale;
   int j = (o->z - o->radius - zoffset)/zscale;
   if (i < 0) i = 0;
   if (j < 0) j = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;
   std::vector<int3d> buckets;
   buckets.push_back(int3d(i,j,0));
   //get all buckets in radius
   while (j < SD_WIDTH && inBucket(i,j,o)){
      int ti = i;
      while (ti < SD_LENGTH && inBucket(ti,j,o)){
         buckets.push_back(int3d(i,j,0));
         ti++;
      }
      j++;
   }

   // Offsets from wall (layering) //
   if (o->parent){
      //find max offset
      float max = -1;
      for (unsigned int a = 0; a < buckets.size(); a++){
         float tmp = offsets[(buckets[a].x)][(buckets[a].y)];
         if (tmp > max) max = tmp;
      }

      //set object offset
      o->offset = max;
      max += 0.002;

      //set bucket offsets
      for (unsigned int a = 0; a < buckets.size(); a++){
         offsets[(buckets[a].x)][(buckets[a].y)] = max;
      }
   }

   //add object to bucket
   all2d.push_back(o);
   o->index = all2d.size() -1;
}

void SData::addFloater(obj2d* o){
   floaters.push_back(o);
   o->classId = FLOATER_ID;
   o->index = floaters.size() -1;
}

float SData::getOffset(float x, float y, float z){
   int i = (x - xoffset)/xscale;
   int j = (z - zoffset)/zscale;
   if (i < 0) i = 0;
   if (j < 0) j = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;

   return offsets[i][j];
}

std::vector<obj*> SData::getObjs(float x, float y, float z){
   int i = (x - xoffset)/xscale;
   int j = (z - zoffset)/zscale;
   if (i < 0) i = 0;
   if (j < 0) j = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;
   float dx = (float)(x - xoffset)/xscale - i;
   float dz = (float)(z - zoffset)/zscale - j;

   std::vector<obj*> bucket = objs[i][j];
   if (dx < .1 && i > 0) {
      //add i -1
      for (unsigned int a = 0; a < objs[i-1][j].size();a++){
         bucket.push_back((objs[i-1][j])[a]);
      }
   } else if (dx > .9 && i < SD_LENGTH - 1){
      //add i + 1
      for (unsigned int a = 0; a < objs[i+1][j].size();a++){
         bucket.push_back((objs[i+1][j])[a]);
      }
   }
   if (dz < .1 && j  > 0) {
      //add j -1
      for (unsigned int a = 0; a < objs[i][j-1].size();a++){
         bucket.push_back((objs[i][j-1])[a]);
      }
   } else if (dz > .9 && j < SD_WIDTH - 1){
      //add j + 1
      for (unsigned int a = 0; a < objs[i][j+1].size();a++){
         bucket.push_back((objs[i][j+1])[a]);
      }
   }

   //add robots
   for (unsigned int a = 0; a < robs.size();a++){
      bucket.push_back(robs[a]);
   }
   return bucket;
}

std::vector<drawable*> SData::getObjsViewable(float x, float y, float z){//get view frustum of objs
   int i = (x - xoffset)/xscale;
   int j = (z - zoffset)/zscale;
   if (i < 0) i = 0;
   if (j < 0) j = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;
   float dx = (float)(x - xoffset)/xscale - i;
   float dz = (float)(z - zoffset)/zscale - j;

   std::vector<drawable*> bucket;
   for (unsigned int a = 0; a < objs[i][j].size();a++){
      bucket.push_back((objs[i][j])[a]);
   }
   if (dx < .1 && i > 0) {
      //add i -1
      for (unsigned int a = 0; a < objs[i-1][j].size();a++){
         bucket.push_back((objs[i-1][j])[a]);
      }
   } else if (dx > .9 && i < SD_LENGTH - 1){
      //add i + 1
      for (unsigned int a = 0; a < objs[i+1][j].size();a++){
         bucket.push_back((objs[i+1][j])[a]);
      }
   }
   if (dz < .1 && j  > 0) {
      //add j -1
      for (unsigned int a = 0; a < objs[i][j-1].size();a++){
         bucket.push_back((objs[i][j-1])[a]);
      }
   } else if (dz > .9 && j < SD_WIDTH - 1){
      //add j + 1
      for (unsigned int a = 0; a < objs[i][j+1].size();a++){
         bucket.push_back((objs[i][j+1])[a]);
      }
   }
   return bucket;
}

std::vector<drawable*> SData::getView(frustum frustum){//gets view frustum and all 2d objects as drawable

   std::vector<drawable*> viewObjs;

   for(unsigned int i = 0; i < all.size(); i++){
     if(!frustum.needsCull(all[i]) || all[i]->classId == PART_ID){
       viewObjs.push_back(all[i]);
       //if (i == 24) frustum.canonic(all[i]->x,all[i]->y,all[i]->z).print();
     }
   }

  // printf("%d\n",all2d.size());
   for(unsigned int i = 0; i < all2d.size(); i++){
      if (all2d[i]->classId < 0) {//delete objs no longer in use
         remove2d(i);
         i--;
      } else {
         if(!frustum.needsCull(all2d[i]) || all2d[i]->classId == ORB_ID){
            viewObjs.push_back(all2d[i]);
         }
      }
   }

   for(unsigned int i = 0; i < floaters.size(); i++){
      viewObjs.push_back(floaters[i]);
   }

   return viewObjs;
}

int SData::bucketX(float x){
   int i = (x - xoffset)/xscale;
   if (i < 0) i = 0;
   if (i > SD_LENGTH-1) i = SD_LENGTH-1;
   return i;
}

int SData::bucketZ(float z){
   int j = (z - zoffset)/zscale;
   if (j < 0) j = 0;
   if (j > SD_WIDTH-1) j = SD_WIDTH-1;
   return j;
}

void updateScreen(frustum frustum, obj *o, float *nx,float *mx,float *ny,float *my){
    //do some clipping
    for (unsigned int i = 0; i < o->surface.size(); i++){
        pnt3d a = frustum.canonic(o->surface[i].verts[0].x,o->surface[i].verts[0].y,o->surface[i].verts[0].z);
        pnt3d b = frustum.canonic(o->surface[i].verts[1].x,o->surface[i].verts[1].y,o->surface[i].verts[1].z);
        pnt3d c = frustum.canonic(o->surface[i].verts[2].x,o->surface[i].verts[2].y,o->surface[i].verts[2].z);
        pnt3d d = frustum.canonic(o->surface[i].verts[3].x,o->surface[i].verts[3].y,o->surface[i].verts[3].z);
        //if (a.x < nx) a.z = 1;
    }
}

std::vector<drawable*> SData::getView(frustum frustum, float x, float y, float z){//gets view frustum and all 2d objects as drawable

   std::vector<drawable*> viewObjs;
   int bucketSize = 2;
   int i = bucketX(x);
   int j = bucketZ(z);

   //float sminx = -1;
   //float smaxx = 1;
   //float sminy = -1;
   //float smaxy = 1;
   std::vector<obj*> bucket;
   for (int k = j - bucketSize; k <= j + bucketSize; k++){      if (k >= 0 && k < SD_WIDTH) {
         for (int h = i - bucketSize; h <= i + bucketSize; h++){
            if (h >= 0 && h < SD_LENGTH) {
               for(unsigned int a = 0; a < objs[h][k].size(); a++){
                  bool added = false;
                  for(unsigned int l = 0; l < bucket.size(); l++){
                     if ((objs[h][k])[a] == bucket[l]){added = true; continue;}
                  }
                  if (!added && !frustum.needsCull((objs[h][k])[a])){
                     bucket.push_back((objs[h][k])[a]);
                     //updateScreen(obj[h][k][a],&sminx,&smaxx,&sminy,&smaxy);
                  }
               }
            }
         }
      }
   }
   //gets vfc bucket around x,y

   //if (i == 24) frustum.canonic(all[i]->x,all[i]->y,all[i]->z).print();

   for(unsigned int i = 0; i < bucket.size(); i++){
       viewObjs.push_back(bucket[i]);
   }

   /*for(unsigned int i = 0; i < all.size(); i++){
     bool added = false;
     for(unsigned int j = 0; j < bucket.size(); j++){
        if (all[i] == bucket[j]){ added = true; continue;}
     }
     if ((!added) && (!frustum.needsCull(all[i]) || all[i]->classId == PART_ID)){
       viewObjs.push_back(all[i]);
     }
   }*/

  // printf("%d\n",all2d.size());
   for(unsigned int i = 0; i < all2d.size(); i++){
      if (all2d[i]->classId < 0) {//delete objs no longer in use
         remove2d(i);
         i--;
      } else {
         if(!frustum.needsCull(all2d[i]) || all2d[i]->classId == ORB_ID){
            viewObjs.push_back(all2d[i]);
         }
      }
   }

   for(unsigned int i = 0; i < floaters.size(); i++){
      viewObjs.push_back(floaters[i]);
   }

   return viewObjs;
}

std::vector<drawable*> SData::getView(){//gets all 2d objects as drawable

   std::vector<drawable*> viewObjs;

   for(unsigned int i = 0; i < all.size(); i++){
     viewObjs.push_back(all[i]);
   }

  // printf("%d\n",all2d.size());
   for(unsigned int i = 0; i < all2d.size(); i++){
      if (all2d[i]->classId < 0) {//delete objs no longer in use
         remove2d(i);
         i--;
      } else {
          viewObjs.push_back(all2d[i]);
      }
   }

   for(unsigned int i = 0; i < floaters.size(); i++){
      viewObjs.push_back(floaters[i]);
   }
   return viewObjs;
}

std::vector<obj*> SData::getViewObjs(frustum frustum){//for picking

   std::vector<obj*> viewObjs;

   for(unsigned int i = 0; i < all.size(); i++){
     if(!frustum.needsCull(all[i])){
       viewObjs.push_back(all[i]);
     }
   }
   return viewObjs;
}

std::vector<obj*> SData::getViewObjs(frustum frustum, float x, float y, float z){//for picking

   std::vector<obj*> viewObjs;
   int bucketSize = 2;
   int i = bucketX(x);
   int j = bucketZ(z);

   //float sminx = -1;
   //float smaxx = 1;
   //float sminy = -1;
   //float smaxy = 1;
   std::vector<obj*> bucket;
   for (int k = j - bucketSize; k <= j + bucketSize; k++){      if (k >= 0 && k < SD_WIDTH) {
         for (int h = i - bucketSize; h <= i + bucketSize; h++){
            if (h >= 0 && h < SD_LENGTH) {
               for(unsigned int a = 0; a < objs[h][k].size(); a++){
                  bool added = false;
                  for(unsigned int l = 0; l < bucket.size(); l++){
                     if ((objs[h][k])[a] == bucket[l]){added = true; continue;}
                  }
                  if (!added && !frustum.needsCull((objs[h][k])[a])){
                     bucket.push_back((objs[h][k])[a]);
                     //updateScreen(obj[h][k][a],&sminx,&smaxx,&sminy,&smaxy);
                  }
               }
            }
         }
      }
   }
   return bucket;
}

std::vector<obj*> SData::getShadowObjs(frustum frustum){
   std::vector<obj*> viewObjs;

   for(unsigned int i = 0; i < sobj.size(); i++){
     if(!frustum.shadowNeedsCull(sobj[i])){
       viewObjs.push_back(sobj[i]);
     }
   }
   return viewObjs;
}

std::vector<obj*> SData::getAllObjs(){
   return all;
}

void SData::colorAll(){
   for(unsigned int i = 0; i < all.size(); i++){
      all[i]->fullColor();
   }
}

int SData::size(){
   return all.size();
}

void SData::draw(){
   glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, -1);
   glLoadName(-1);//used for picking
   for (int j = 0; j < SD_LENGTH; j++){
      for (int i = 0; i < SD_LENGTH; i++){
         glBegin(GL_LINES);
         glVertex3f(xoffset + xscale*i,j*xscale,zoffset);
         glVertex3f(xoffset + xscale*i,j*xscale,zoffset + zscale*SD_WIDTH);
         glEnd();
      }
      for (int i = 0; i < SD_WIDTH; i++){
         glBegin(GL_LINES);
         glVertex3f(xoffset,                       j*xscale,   zoffset + zscale*i);
         glVertex3f(xoffset + xscale*SD_LENGTH, j*xscale,   zoffset + zscale*i);
         glEnd();
      }
   }
   for (int i = 0; i < SD_LENGTH; i++){
      for (int j = 0; j < SD_WIDTH; j++){
         glBegin(GL_LINES);
         glVertex3f(xoffset + xscale*i,0,                  zoffset + zscale*j);
         glVertex3f(xoffset + xscale*i,SD_LENGTH*xscale,zoffset + zscale*j);
         glEnd();
      }
   }
   glPopMatrix();
}

void SData::drawNet(){
   glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, -1);
   glLoadName(-1);//used for picking
   glColor3f(1,1,1);
   //glBegin(GL_POINTS);
   glBegin(GL_LINES);
   glLineWidth(5);
   for(unsigned int i = 0; i < nodes.size(); i++){
      glColor3f(.5+((float)i/nodes.size()),1,1-((float)(i%10)/20.0));
      for(unsigned int j = 0; j < nodes[i]->next.size(); j++){
         glVertex3f(nodes[i]->x,nodes[i]->y,nodes[i]->z);
         glVertex3f(nodes[i]->next[j]->x,nodes[i]->next[j]->y,nodes[i]->next[j]->z);
      }
   }
   glEnd();
   glPopMatrix();
}

iNode * SData::getNode(iNode *n){
   if (!n) return nodes[(rand()%nodes.size())];
   return n->next[(rand()%(n->next.size()))];
}

void SData::createNet(){
   nodes.clear();
   int start,cur;
   cur = 0;
   for(unsigned int i = 0; i < all.size(); i++){
      for(unsigned int j = 0; j < all[i]->surface.size(); j++){
         start = nodes.size();
         for(unsigned int k = 0; k < all[i]->surface[j].verts.size(); k++){
            cur = nodes.size();
            nodes.push_back(new iNode(all[i]->surface[j].verts[k].x + all[i]->x,all[i]->surface[j].verts[k].y + all[i]->y,all[i]->surface[j].verts[k].z + all[i]->z));
            if (k != 0) {
               nodes[cur]->add(nodes[cur-1]);
               nodes[cur-1]->add(nodes[cur]);
            }
         }
         nodes[cur]->add(nodes[start]);
         nodes[start]->add(nodes[cur]);
      }
   }
   //printf("Connecting %d nodes\n", nodes.size());
   //connectNet();
}


void SData::connectNet(){
   unsigned int curlen = nodes.size();//size dynamic, only want size now!
   for(unsigned int i = 0; i < curlen; i++){
      for(unsigned int j = 0; j < nodes[i]->next.size(); j++){
         //printf("**netting path %d:%d\n",i,j);
         netPath(nodes[i],nodes[i]->next[j]);
      }
      nodes[i]->state = true;
   }
}

bool inRad(float a, float b, float r){
   return a*a + b*b < r;
}

void SData::netPath(iNode* a, iNode* b){
   if (a->state || b->state) return;
   float rad = a->dist(b);
   std::vector<iNode*> branch;
   if (a->x == b->x){
      if (a->y == b->y){
         //XY
         //printf("XY\n");
         for(unsigned int i = 0; i < nodes.size(); i++){
            if (inRad((nodes[i]->x)-(a->x), (nodes[i]->y)-(a->y), rad)){
               if ((nodes[i]->z < max(a->z,b->z)+rad) && (nodes[i]->z > min(a->z,b->z)-rad)){
               //node is in cylinder
                  unsigned int j;
                  for(j = 0; j < branch.size(); j++){
                     if (branch[j]->z == nodes[i]->z){//existing branch
                        branch[j]->add(nodes[i]);
                        nodes[i]->add(branch[j]);
                        break;
                     }
                  }
                  if (j == branch.size()){//make new branch
                     //printf("XY Make new branch\n");
                     nodes.push_back(new iNode(a->x,a->y,nodes[i]->z));
                     iNode* cur = nodes[(nodes.size()-1)];
                     branch.push_back(cur);
                     cur->state = true;
                     cur->add(nodes[i]);//brance to leaf
                     nodes[i]->add(cur);
                  }
               }
            }
         }
         //printf("XY branches:%d\n",branch.size());
         //add branches
         iNode* start = a;
         iNode* end = b;
         iNode* mid;
         int midi;
         if (start->z < end->z){
            mid = start;
            start = end;
            end = mid;
         }
         while (branch.size()>0){
            mid = branch[0];
            midi = 0;
            for(unsigned int i = 1; i < branch.size(); i++){ //gets closest to start
               if ((start->z - branch[i]->z) < (start->z - mid->z)) {
                  mid = branch[i];
                  midi = i;
               }
            }
            //break link
            start->remove(end);
            end->remove(start);
            //insert node
            start->add(mid);
            mid->add(start);
            mid->add(end);
            end->add(mid);
            start = mid;
            branch.erase(branch.begin()+midi);
         }
      }
      else if (a->z == b->z){
         //XZ
         //printf("XZ\n");
         for(unsigned int i = 0; i < nodes.size(); i++){
            if (inRad((nodes[i]->x)-(a->x), (nodes[i]->z)-(a->z), rad)){
               if ((nodes[i]->y < max(a->y,b->y)+rad) && (nodes[i]->y > min(a->y,b->y)-rad)){
               //node is in cylinder
                  unsigned int j;
                  for(j = 0; j < branch.size(); j++){
                     if (branch[j]->y == nodes[i]->y){//existing branch
                        branch[j]->add(nodes[i]);
                        nodes[i]->add(branch[j]);
                        break;
                     }
                  }
                  if (j == branch.size()){//make new branch
                     //printf("XZ Make new branch\n");
                     nodes.push_back(new iNode(a->x,nodes[i]->y,a->z));
                     iNode* cur = nodes[(nodes.size()-1)];
                     branch.push_back(cur);
                     cur->state = true;
                     cur->add(nodes[i]);//brance to leaf
                     nodes[i]->add(cur);
                  }
               }
            }
         }
         //printf("XZ branches:%d\n",branch.size());
         //add branches
         iNode* start = a;
         iNode* end = b;
         iNode* mid;
         int midi;
         if (start->y < end->y){
            mid = start;
            start = end;
            end = mid;
         }
         while (branch.size()>0){
            mid = branch[0];
            midi = 0;
            for(unsigned int i = 1; i < branch.size(); i++){ //gets closest to start
               if ((start->y - branch[i]->y) < (start->y - mid->y)) {
                  mid = branch[i];
                  midi = i;
               }
            }
            //break link
            start->remove(end);
            end->remove(start);
            //insert node
            start->add(mid);
            mid->add(start);
            mid->add(end);
            end->add(mid);
            start = mid;
            branch.erase(branch.begin()+midi);
         }
      }
   } else if (a->y == b->y){
      if (a->z == b->z){
         //YZ
         //printf("YZ\n");
         for(unsigned int i = 0; i < nodes.size(); i++){
            if (inRad((nodes[i]->y)-(a->y), (nodes[i]->z)-(a->z), rad)){
               if ((nodes[i]->x < max(a->x,b->x)+rad) && (nodes[i]->x > min(a->x,b->x)-rad)){
               //node is in cylinder
                  unsigned int j;
                  for(j = 0; j < branch.size(); j++){
                     if (branch[j]->x == nodes[i]->x){//existing branch
                        branch[j]->add(nodes[i]);
                        nodes[i]->add(branch[j]);
                        break;
                     }
                  }
                  if (j == branch.size()){//make new branch
                     //printf("YZ Make new branch\n");
                     nodes.push_back(new iNode(nodes[i]->x,a->y,a->z));
                     iNode* cur = nodes[(nodes.size()-1)];
                     branch.push_back(cur);
                     cur->state = true;
                     cur->add(nodes[i]);//brance to leaf
                     nodes[i]->add(cur);
                  }
               }
            }
         }
         //printf("YZ branches:%d\n",branch.size());
         //add branches
         iNode* start = a;
         iNode* end = b;
         iNode* mid;
         int midi;
         if (start->x < end->x){
            mid = start;
            start = end;
            end = mid;
         }
         while (branch.size()>0){
            //printf("%d",branch.size());
            mid = branch[0];
            midi = 0;
            for(unsigned int i = 1; i < branch.size(); i++){ //gets closest to start
               if ((start->x - branch[i]->x) < (start->x - mid->x)) {
                  mid = branch[i];
                  midi = i;
               }
            }
            //break link
            start->remove(end);
            end->remove(start);
            //insert node
            start->add(mid);
            mid->add(start);
            mid->add(end);
            end->add(mid);
            start = mid;
            branch.erase(branch.begin()+midi);
         }
      }
   }
}


