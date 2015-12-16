#if ! defined _IMPORTMAP_H
#define _IMPORTMAP_H 1

#include "util.h"
#include "obj.h"
#include "orb.h"
#include "trigger.h"
#include "mobj.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "sdata.h"
#include "importMap.h"
#include "player.h"
#include "tex.h"
#include "robot.h"
#include "level.h"
#include "tree.h"
#include "hud.h"

extern HUD* hud;

#if 1
#include <GL/glut.h>
#endif

#if 0
#include <GLUT/glut.h>
#include <OPENGL/gl.h>
#include <OPENGL/glext.h>
#endif

class importMap {
  public:
     char* mapName;
     importMap(char* in_mapName):mapName(in_mapName){};
     virtual ~importMap(){}
     void import();
     void createDetail();
     void createFloaters();
};

#endif
