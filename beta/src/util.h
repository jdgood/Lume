#if ! defined _UTIL_H
#define _UTIL_H 1

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <assert.h>
#include <map>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "GL/glew.h"
#include <GL/glut.h>

#define DEG2RAD 0.0174532925//degrees to radians conversion
#define RAD2DEG 57.295779579
#define TIMERINT 10 //timer interval

//classId definitions
#define OBJ_ID 1
#define ORB_ID 2
#define GOBJ_ID 3
#define OBJ2D_ID 4
#define RAMP_ID 5
#define DETAIL2D_ID 6
#define GOBJ2D_ID 7
#define SKY_ID 8
#define ROB_ID 9
#define TELEPORT_ID 10
#define GLASS_ID 11
#define GROUND_ID 12
#define FLOATER2D_ID 13
#define FOG_ID 14
#define TRIGGER_ID 15
#define MODEL_ID 16
#define MOVINGOBJ_ID 17
#define PART_ID 18
#define TREE_ID 19
#define FLOATER_ID 20
#define BB2D_ID 21

#define NUM_STAGES 2

#define FATALERROR(cond, statement) \
   if (cond) { \
      perror(statement); \
      exit(EXIT_FAILURE); \
   }

#define CONTERROR(cond, statement) \
   if (cond) { \
      perror(statement); \
   }

extern bool frustumMode;
extern bool wireframe;
extern bool allLevels;
extern bool basic;
extern bool glow;
extern bool BLUR_ON;
extern bool BLOOM_ON;
extern bool models;
extern int GH;
extern int GW;
extern bool keystates[256];
extern bool fullLoad;
extern char curStage[128];
#endif
