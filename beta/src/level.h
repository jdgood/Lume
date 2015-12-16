#if ! defined _LEVEL_H
#define _LEVEL_H 1

#include "cam.h"
#include "obj.h"
#include "model.h"
#include "shader.h"
#include "sdata.h"
#include "objective.h"
#include "player.h"
#include <vector>

extern Shader* shader;
extern SData world;
extern bool editmode;
extern pnt3d edgeColor;
extern float waveEffect[8];
extern float glowBrightness;
extern Player character;
extern pnt3d cinematicCurrent;

using namespace std;

class Level {
      public:
      pnt3d color;
      vector<Objective*> objectives;
      char levelName[128];
      int frame;
      int robotsKilled;
      int energyCollected;
      int lastEnergy;
      int totalEnergy;
      int blocksBuilt;
      pnt3d center;
      bool activeMovingPlatforms;
      bool levelComplete;
      bool displayObjectives;
      Level();
      Level(vector<Objective*> goals, char* name);
      const char * music();
      void load();
      void update(float dt);
};
#endif

