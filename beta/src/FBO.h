#if ! defined _FBO_H
#define _FBO_H 1

#include "util.h"
#include "shader.h"

extern Shader *bloom;
//extern Shader *pixeld;

class FBO {
   public:
      FBO();
      ~FBO();
      
	   GLuint frame, depth, texid, texid2, texid3;
	   int width, height;
	
	   int state;//0-norm,1-flash
      int blurAmount;
      float blurIntensity;
      float blurDecay;
      float blurOffset;
      void flash();
      void smallflash();
      void update(float dt);
      void glowEdges();
      void draw();
      void resize();
};

//defaults
#define BLUR_AMOUNT 1
#define BLUR_INTENSITY 1.0
#define BLUR_DECAY 0.01
#define BLUR_OFFSET 0.005

//edge blur defaults
#define EDGE_AMOUNT 4 // how many layers
#define EDGE_INTENSITY 3.0 //how bright
#define EDGE_OFFSET 0.002 //offset between blur layers

#endif
