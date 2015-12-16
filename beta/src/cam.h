#if ! defined _CAM_H
#define _CAM_H 1

#include "util.h"
#include "obj.h"

#define CAM_UP 2 
extern pnt3d character_start;
extern pnt3d objective_start;
extern pnt3d cinematicGoal;
extern vector<pnt3d*> cam_path;

class cam{
   public:
      vec3d pos;
      float theta;
      float phi;
      float offset;
      float* px;
      float* py;      
      float* pz;
      int zoomlevel;
      bool cinematic;
      bool intro;
      bool introStart;
      bool pathFinished;
      pnt3d start;
      pnt3d finish;
      float cine;
      unsigned int currentPathIndex;
      cam();
      cam(float* plx, float* ply, float* plz);
      cam(float px, float py, float pz, float x, float y, float z);
      pnt3d upVector();
      pnt3d pickPos();
      void reset();
      void lookAt(float* plx, float* ply, float* plz);
      void zoom(float z);
      void setup();
      void update(float dx, float dy);
      void updateCinematic();
      void zoomin();
      void zoomout(obj in);
      void initPath();
      void introScene(pnt3d start, pnt3d finish);
};

#endif
