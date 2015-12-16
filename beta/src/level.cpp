#include "level.h"

Level::Level() : objectives() {
   strcpy(levelName, " ");
   activeMovingPlatforms = false;
   levelComplete = false;//this doesnt work for some reason
   switch(rand()%3){
      case 0 :color = pnt3d(.5,.7,1); break;//blue was .5 1 1
      case 1 :color = pnt3d(1,.7,.3); break;//red
      case 2: color = pnt3d(.3,1,.3); break;//green
   }
   energyCollected = 0;
   totalEnergy = 0;
   lastEnergy = 0;
}


Level::Level(vector<Objective*> goals, char* name) : objectives(goals) {
   strcpy(levelName, name);
   activeMovingPlatforms = false;
   levelComplete = false;//this doesnt work for some reason
   switch(rand()%3){
      case 0 :color = pnt3d(.5,.7,1); break;//blue was .5 1 1
      case 1 :color = pnt3d(1,.7,.3); break;//red
      case 2: color = pnt3d(.3,1,.3); break;//green
   }
   energyCollected = 0;
   totalEnergy = 0;
   lastEnergy = 0;
}

void Level::load(){
   edgeColor = color;
   robotsKilled = 0;
   //energyCollected = 0;
   //totalEnergy = 0;
   //lastEnergy = 0;
}

const char * Level::music(){
   if (levelName){
      if (strcmp(levelName,(char *)"tutorial") == 0){
         return "bg1.ogg";
      } else if (strcmp(levelName, (char *)"ghetto") == 0){
         return "timeCrunch.ogg";
      } else if (strcmp(levelName, (char *)"city") == 0){
         return "bg3.ogg";
      } else if (strcmp(levelName, (char *)"suburbs") == 0){
         return "bg2.ogg";
      } else if (strcmp(levelName, (char *)"skyline") == 0){
         return "bg4.ogg";
      } else if (strcmp(levelName, (char *)"overworld") == 0){
         return "endCred.ogg";
      }
   }
   return "bg1.ogg";
}

void Level::update(float dt){
   //glowBrightness = 0.0;
   /*if (objectives.size() < 1){
      glowBrightness = .5;  
   } else {
      for (unsigned int i = 0; i < objectives.size(); i++){
         glowBrightness+= (float)(objectives[i]->currentComplete)/(objectives[i]->totalNeeded);
      }
      glowBrightness /= objectives.size();
   }*/
   if (totalEnergy) { 
      if (glowBrightness > (float)energyCollected/totalEnergy) glowBrightness-=0.005*(dt);
      if (lastEnergy != energyCollected) {
         lastEnergy = energyCollected;
         glowBrightness += 0.5;
      }
   }
   else { glowBrightness = .5;}
   //printf("%d/%d\n",energyCollected,totalEnergy);
   frame++;
   if (frame > 200 || frame < 0) frame = 0;
   for (int i = 0; i < 8; i++){
      waveEffect[i] = (cos(3.14159*(frame + 24*(i))/100) + 1)*(.10+glowBrightness*1.35);
   }
   unsigned int objectivecount = 0;
   for(unsigned int i = 0; i < objectives.size(); i++){
     objectives[i]->update(dt);
     if(objectives[i]->isComplete()){
       objectivecount++;
     }
   }
   if(!activeMovingPlatforms){
     if(objectives[0]->isComplete()){
       activeMovingPlatforms = true;
       for(unsigned int i = 0; i < world.all.size(); i++){
         if(world.all[i]->classId == MOVINGOBJ_ID){
           world.all[i]->setActive();
         }
       }
     }
   }
   //printf("%s\n", (levelComplete)?"true":"false");
   if(!levelComplete && objectivecount == objectives.size()){
     levelComplete = true;
     //do cinematic temp one below
     cinematicCurrent.x = character.x;
     cinematicCurrent.y = character.y;
     cinematicCurrent.z = character.z;
     camera.lookAt(&cinematicCurrent.x,&cinematicCurrent.y,&cinematicCurrent.z);
     //camera.lookAt(&center.x,&center.y,&center.z);
     camera.cinematic = true;  
     camera.cine = 0;
     camera.offset = 2;
     world.colorAll();
     if(character.stage){
       character.stage++;
       if (!strcmp(levelName, (char*)"KOG")) {
          character.loadingStage = 50;
       }
       else {
          character.loadingStage = 350;
       }
     }
   }
}
