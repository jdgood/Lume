#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "gobj.h"
#include "obj.h"
#include "gameState.h"
#include "player.h"
#include "level.h"
#include "objective.h"
#include "trigger.h"

#if 1
#include <GL/glut.h>
#endif

#if 0
#include <GLUT/glut.h>
#include <OPENGL/gl.h>
#include <OPENGL/glext.h>
#endif

extern char curStage[128];
extern Player character;
extern cam camera;
extern vector<Level> levels;
extern vector<pnt3d*> cam_path;

void gameState::save(char* saveName) {
   FILE* game;
   game = fopen(saveName, "w");
   char buffer[1024];
   
   sprintf(buffer, "%s", curStage);
   fputs(buffer, game);
   fputs("\n", game);
   
   sprintf(buffer, "%f %f %f %f %f %f %d %d %f", character.x, character.y, character.z, character.checkpoint.x, 
      character.checkpoint.y, character.checkpoint.z, character.level, character.curenergy, character.exp);
   fputs(buffer, game);
   fputs("\n", game);

   for(int i = 0; i < world.size(); i++){
      if(world.all[i]->classId == GOBJ_ID){
         sprintf(buffer, "gobjs %f %f %f %f %f %f %f %f %f %f %f %f", world.all[i]->x, 
            world.all[i]->y, world.all[i]->z, world.all[i]->getDir().x, world.all[i]->getDir().y,
            world.all[i]->getDir().z, world.all[i]->xmin, world.all[i]->ymin, world.all[i]->zmin,
            world.all[i]->xmax, world.all[i]->ymax, world.all[i]->zmax);
         fputs(buffer, game);
         fputs("\n", game);
      }
   }
   
   for(unsigned int i = 0; i < world.all2d.size(); i++){
      if(world.all2d[i]->classId == GOBJ2D_ID){
         sprintf(buffer, "obj2d %d %f %f %f %f %s", world.all2d[i]->getParentI(), world.all2d[i]->getX(), world.all2d[i]->getY(), world.all2d[i]->getZ(), world.all2d[i]->getSize(), getTexName(world.all2d[i]->getTexId()));
         fputs(buffer, game);
         fputs("\n", game);
      }
      else if(world.all2d[i]->classId == SRC_ID){
         if(world.all2d[i]->getDrained()){
            sprintf(buffer, "%d ", i);
            fputs(buffer, game);
            fputs("\n", game);
         }
      }
   }
   
   //level save works but saves after loading for some reason....brain hurts ahhh
   /*for(unsigned int i = 0; i < levels.size(); i++){
      sprintf(buffer, "level %s %d %d %d %d\n", levels[i].levelName, levels[i].activeMovingPlatforms, levels[i].levelComplete, 
               levels[i].displayObjectives, levels[i].objectives.size());
      fputs(buffer, game);
      for(unsigned int j = 0; j < levels[i].objectives.size(); j++){
         sprintf(buffer, "objective %s %d %d %d %d\n", levels[i].objectives[j]->collectableName, levels[i].objectives[j]->currentComplete, 
                  levels[i].objectives[j]->insightPosted, levels[i].objectives[j]->played, levels[i].objectives[j]->triggers.size());
         fputs(buffer, game);
         sprintf(buffer, "%s\n", levels[i].objectives[j]->objectiveName);
         fputs(buffer, game);
         for(unsigned int k = 0; k < levels[i].objectives[j]->triggers.size(); k++){
            sprintf(buffer, "trigger %f %f %f %f %f %f %d %d %d\n", 
                     levels[i].objectives[j]->triggers[k]->x, levels[i].objectives[j]->triggers[k]->y, levels[i].objectives[j]->triggers[k]->z, 
                     levels[i].objectives[j]->triggers[k]->xmax - levels[i].objectives[j]->triggers[k]->xmin, 
                     levels[i].objectives[j]->triggers[k]->ymax - levels[i].objectives[j]->triggers[k]->ymin, 
                     levels[i].objectives[j]->triggers[k]->zmax - levels[i].objectives[j]->triggers[k]->zmin, levels[i].objectives[j]->triggers[k]->hit, 
                     levels[i].objectives[j]->triggers[k]->updateDetail, levels[i].objectives[j]->triggers[k]->downloadbar);
            fputs(buffer, game);
         }
      }
   }*/

   fclose(game);
}

int gameState::load(char* saveName) {
  FILE* game;
  char loadname[128];
  vector<int> setdrained;
  game = fopen(saveName, "r");
  if(game==NULL){
    //printf("File Not Found : %s\n", "saves/game1");
    return 0;
  }
  //levels.clear();
  currentLevel = NULL;

  character = Player();

  char buffer[1024];
  char* token;
  
  fgets (buffer , 1024 , game);
  
  token = strtok(buffer, "\n");
  
  strcpy(loadname, token);
  
  fgets (buffer , 1024 , game);
   
  token = strtok(buffer, " ");//x
  float charx = atof(token);
  token = strtok(NULL, " ") ;//y
  float chary = atof(token);
  token = strtok(NULL, " ");//z
  float charz = atof(token);
  token = strtok(NULL, " ");//cpx
  float charcpx = atof(token);
  token = strtok(NULL, " ");//cpy
  float charcpy = atof(token);
  token = strtok(NULL, " ");//cpz
  float charcpz = atof(token);
  token = strtok(NULL, " ");//level
  int charlevel = atoi(token);
  token = strtok(NULL, " ");//energy
  int charcurenergy = atoi(token);
  token = strtok(NULL, "\n");//exp
  float charexp = atof(token);
 
  fgets (buffer , 1024 , game);
    
  while (!feof(game)){//will be a loop to create gobjs, set drained for cps and billboards
    token = strtok(buffer, " ");//gobjs or sourcenumber
    if(!strcmp(token, "gobjs")){
      token = strtok(NULL, " ") ;//x
      float x = atof(token);
      token = strtok(NULL, " ") ;//y
      float y = atof(token);
      token = strtok(NULL, " ");//z
      float z = atof(token);
      token = strtok(NULL, " ");//cpx
      float dirx = atof(token);
      token = strtok(NULL, " ");//cpy
      float diry = atof(token);
      token = strtok(NULL, " ");//cpz
      float dirz = atof(token);
      
      GrowingObj* temp = newGrowingCube(x,y,z,dirx,diry,dirz);
      
      token = strtok(NULL, " ") ;//x
      float xmin = atof(token);
      token = strtok(NULL, " ") ;//y
      float ymin = atof(token);
      token = strtok(NULL, " ");//z
      float zmin = atof(token);
      token = strtok(NULL, " ");//cpx
      float xmax = atof(token);
      token = strtok(NULL, " ");//cpy
      float ymax = atof(token);
      token = strtok(NULL, "\n");//cpz
      float zmax = atof(token);

      temp->xmin = xmin;
      temp->ymin = ymin;
      temp->zmin = zmin;
      temp->xmax = xmax;
      temp->ymax = ymax;
      temp->zmax = zmax;
      temp->resize();
      world.add(temp);
      world.addShadow(world.all[world.size()-1]);
    }
    else if(!strcmp(token, "obj2d")){
      token = strtok(NULL, " ");//parent id
      int parentid = atoi(token);
      token = strtok(NULL, " ") ;//x
      float x = atof(token);
      token = strtok(NULL, " ");//y
      float y = atof(token);
      token = strtok(NULL, " ");//z
      float z = atof(token);
      token = strtok(NULL, " ");//size(height == width)
      float size = atof(token);
      char type[64];
      strcpy(type, token);
      token = strtok(NULL, "\n");//texture name
      char textureName[64];
      strcpy(textureName, token);
       
      world.add(new obj2d(x, y, z, world.all[parentid], &(camera.pos), LoadTexture(textureName), size, 1));
    }
    else if(!strcmp(token, "level")){
      char levelname[128];
      bool activeMovingPlatforms;
      bool levelComplete;
      bool displayobjectives;
      int objectivecount;
      
      token = strtok(NULL, " ");//levelname
      strcpy(levelname, token);
      token = strtok(NULL, " ");//moving platforms
      activeMovingPlatforms = atoi(token);
      token = strtok(NULL, " ");//level complete
      levelComplete = atoi(token);
      token = strtok(NULL, " ");//display objectives
      displayobjectives = atoi(token);
      token = strtok(NULL, "\n");//objective count
      objectivecount = atoi(token);
      
      levels.push_back(Level());
      levels[levels.size() - 1].activeMovingPlatforms = activeMovingPlatforms;
      levels[levels.size() - 1].levelComplete = levelComplete;
      levels[levels.size() - 1].displayObjectives = displayobjectives;
      strcpy(levels[levels.size()-1].levelName, levelname);
      if(!strcmp(levelname, loadname)){
         currentLevel = &levels[levels.size() - 1];
      }
      //printf("-----------------------%d-----------------\n", objectivecount);
      for(int i = 0; i < objectivecount; i++){
        char collectableName[128];
        int currentComplete;
        bool insightPosted;
        bool played;
        int triggercount;
        char objectiveName[128];
        vector<trigger*> temptriggers;
        
        fgets(buffer , 1024 , game);
        token = strtok(buffer, " ");//objective
        token = strtok(NULL, " ");//collectable
        strcpy(collectableName, token);
        token = strtok(NULL, " ");//currrent complete
        currentComplete = atoi(token);
        token = strtok(NULL, " ");//insight posted
        insightPosted = atoi(token);
        token = strtok(NULL, " ");//played
        played = atoi(token);
        token = strtok(NULL, "\n");//trigger count
        triggercount = atoi(token);
        fgets(buffer , 1024 , game);
        token = strtok(buffer, "\n");//objective text
        strcpy(objectiveName, token);
        
        Objective* tempobjective;
        //create and add tempobjective to tempobjectives
        tempobjective = new Objective(currentComplete, triggercount, objectiveName, collectableName);
        tempobjective->insightPosted = insightPosted;
        tempobjective->played = played;
        levels[levels.size() - 1].objectives.push_back(tempobjective);
        //printf("added objective to %s\n", levelname);
        
        for(int j = 0; j < triggercount; j++){
          float x, y, z, dx, dy, dz;
          bool hit;
          bool updateDetail;
          bool downloadbar;
          fgets(buffer , 1024 , game);
          token = strtok(buffer, " ");//trigger
          token = strtok(NULL, " ");//x
          x = atof(token);
          token = strtok(NULL, " ");//y
          y = atof(token);
          token = strtok(NULL, " ");//z
          z = atof(token);
          token = strtok(NULL, " ");//dx
          dx = atof(token);
          token = strtok(NULL, " ");//dy
          dy = atof(token);
          token = strtok(NULL, " ");//dz
          dz = atof(token);
          token = strtok(NULL, " ");//hit
          hit = atoi(token);
          token = strtok(NULL, " ");//update detail
          updateDetail = atoi(token);
          token = strtok(NULL, "\n");//downloadbar
          downloadbar = atoi(token);
          trigger* temptrigger = new trigger(x,y,z,dx,dy,dz,collectableName);
          temptrigger->hit = hit;
          temptrigger->updateDetail = updateDetail;
          temptrigger->downloadbar = downloadbar;
          tempobjective->triggers.push_back(temptrigger);
        }
      }
    }
    else{//source
      setdrained.push_back(atoi(token));
    }
    
    fgets (buffer , 1024 , game);
  }
  fclose(game);
  
  importMap(loadname).import();
  for(unsigned int k = 0; k < setdrained.size(); k++){
    world.all2d[setdrained[k]]->setDrained(true);
  }
  cam_path.clear();
  camera.introStart = false;
  camera.intro = false;
  camera.pathFinished = true;
  camera.currentPathIndex = 100000;
  character.x = charx;
  character.y = chary;
  character.z = charz;
  character.setCheckpoint(charcpx, charcpy, charcpz);
  character.setLevel(charlevel);
  character.setEnergy(charcurenergy);
  character.exp = charexp;
  
  camera = cam(&(character.x),&(character.y),&(character.z));
  character.gameStarted = true;
  //printf("%d\n", currentLevel->objectives[0]->isComplete());
  
  return 1;
}
