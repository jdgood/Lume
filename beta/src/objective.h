#if ! defined _OBJECTIVE_H
#define _OBJECTIVE_H 1

#include "cam.h"
#include "obj.h"
#include "model.h"
#include "shader.h"
#include "sdata.h"
#include "trigger.h"

extern Player character;
extern Shader* shader;
extern SData world;
extern bool editmode;
class Objective {
      public:
      
      int currentComplete;
      int totalNeeded;
      
      char objectiveName[128];
      
      char collectableName[128];
      bool insightPosted;
      bool played;
      std::vector<trigger*> triggers;
      
      Objective();
      Objective(int curr, int needed, char* goalName, char* collectName, std::vector<trigger*> trigs);
      Objective(int curr, int needed, char* goalName, char* collectName);
      

      void updateDetail();
      bool isComplete();
      void update(float dt);
      void draw(float x,float y);
      void intro();
      
};
#endif

