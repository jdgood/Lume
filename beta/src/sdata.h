#if ! defined _SDATA_H
#define _SDATA_H 1

#include "util.h"
#include "obj.h"
#include "frustum.h"

#define SD_LENGTH 50
#define SD_WIDTH 50

typedef struct iNode {
  float x;
  float y;
  float z;
  bool state;
  std::vector<iNode*> next;
  iNode():x(0),y(0),z(0),state(false){};
  iNode(float in_x,float in_y,float in_z):x(in_x),y(in_y),z(in_z),state(false){};
  void add(iNode* nod){
     for(unsigned int i = 0; i < next.size(); i++){
         if (nod == next[i]) return;
     }
     next.push_back(nod);
  }
  void remove(iNode* nod){
     for(unsigned int i = 0; i < next.size(); i++){
         if (nod == next[i]){
            next.erase(next.begin()+i);
            break;
         }
     }
  }
  float dist(iNode* nod){
      return sqrt((nod->x-x)*(nod->x-x) + (nod->y-y)*(nod->y-y) + (nod->z-z)*(nod->z-z));  
  }
  float dist(float nx,float ny, float nz){
      return sqrt((nx-x)*(nx-x) + (ny-y)*(ny-y) + (nz-z)*(nz-z));  
  }
} iNode;

class SData {
  public:
     int xscale;// SD_XSCALE 20
     int zscale;// SD_ZSCALE 20
     int xoffset;// SD_XOFFSET -10
     int zoffset;// SD_ZOFFSET -10
     std::vector<obj*> all;
     std::vector<obj2d*> all2d;
     std::vector<obj2d*> floaters;
     std::vector<obj*> robs;
     std::vector<obj*> sobj;
     std::vector<obj*> objs[SD_LENGTH][SD_WIDTH];
     float offsets[SD_LENGTH][SD_WIDTH];
     SData();
     void resize(float offsetx, float offsetz, float lengthx, float lengthz);
     void removeAll();
     obj* get(int index);
     void remove(int index);
     void remove(obj* o);
     void remove2d(int index);
     bool inBucket(int i, int j, obj* o);
     void add(obj* o);
     void update(int i);
     bool inBucket(int i, int j, obj2d* o);
     void add(obj2d* o);
     void addFloater(obj2d* o);
     void addRobot(obj* o);
     void addMovingObj(obj* o);
     void addShadow(obj* o);
     float getOffset(float x, float y, float z);
     int bucketX(float x);
     int bucketZ(float z);
     std::vector<drawable*> getView(frustum frustum, float x, float y, float z);
     std::vector<obj*> getObjs(float x, float y, float z);
     std::vector<drawable*> getObjsViewable(float x, float y, float z); 
     std::vector<drawable*> getView(frustum frustum);
     std::vector<drawable*> getView();
     std::vector<obj*> getShadowObjs(frustum frustum);
     std::vector<obj*> getViewObjs(frustum frustum);
     std::vector<obj*> getViewObjs(frustum frustum, float x, float y, float z);
     std::vector<obj*> getAllObjs();
     std::vector<iNode*> nodes;
     void colorAll();
     int size();
     void draw();
     void drawNet();
     iNode * getNode(iNode *n);
     void createNet();
     void connectNet();
     void netPath(iNode* a, iNode* b);
};

#endif
