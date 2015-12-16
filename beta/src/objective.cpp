#include "objective.h"
#include "level.h"
extern char curStage[128];
extern Level *currentLevel;

Objective::Objective()
  :  currentComplete(0), totalNeeded(0), triggers() {played = false; insightPosted = false; strcpy(objectiveName, " "); strcpy(collectableName, " "); }


Objective::Objective(int curr, int needed, char* goalName, char* collectName, std::vector<trigger*> trigs)
  :  currentComplete(curr), totalNeeded(needed), triggers(trigs) {played = false; insightPosted = false; strcpy(objectiveName, goalName); strcpy(collectableName, collectName); }
  
Objective::Objective(int curr, int needed, char* goalName, char* collectName)
  :  currentComplete(curr), totalNeeded(needed), triggers() {played = false; insightPosted = false; strcpy(objectiveName, goalName); strcpy(collectableName, collectName); }

bool Objective::isComplete(){
  return (currentComplete >= totalNeeded);
}

void Objective::intro(){
  if(!played){
    character.control(triggers[triggers.size()-1]);
    camera.cinematic = true;
    camera.cine = 0;
    camera.offset = 2;
  }
  played = true;
}

void Objective::updateDetail() {
   FILE *map;
   char mapbuffer[1024];
   char buffer[1024];
   char *token;

 sprintf(mapbuffer, "maps/%s/detail", curStage);
  map = fopen(mapbuffer, "r");
  if(map==NULL){
    //printf("No detail for: %s\n",mapName);
    return;
  }

     fgets (buffer , 1024 , map);
     while (!feof(map)){
       token = strtok(buffer, " ");//parent id
       int parentid = atoi(token) + 1;
       token = strtok(NULL, " ");//x
       float x = atof(token);
       token = strtok(NULL, " ");//y
       float y = atof(token);
       token = strtok(NULL, " ");//z
       float z = atof(token);
       token = strtok(NULL, " ");//size(height == width)
       float size = atof(token);
       char type[64];
       strcpy(type, token);
       token = strtok(NULL, " ");//texture name
       char textureName[64];
       strcpy(textureName, token);
       token = strtok(NULL, " ");//objective name
       char tempOName[64];
       strcpy(tempOName, token);
       token = strtok(NULL, "\n");//objective num
       int tempONum = atoi(token);
       fgets (buffer , 1024 , map);
       
       if(strcmp(tempOName, "null")){
        //add level objective logic here based on currentlevel data
           if(!strcmp(tempOName, collectableName)){
              if(triggers[tempONum-1]->updateDetail){
                 //printf("nyanish:%d\n",parentid);
                 if(!strncmp(textureName, (char*)"textures/Vine_/", 15)){
                    world.add(new obj2d(x, y, z, world.all[parentid], &(camera.pos), (char*)"Vine_", 30, size));
                 }
                 else if(!strncmp(textureName, (char*)"textures/Vine2_/", 16)){
                    world.add(new obj2d(x, y, z, world.all[parentid], &(camera.pos), (char*)"Vine2_", 30, size));
                 }
                 else if(!strncmp(textureName, (char*)"textures/Vine3_/", 16)){
                    world.add(new obj2d(x, y, z, world.all[parentid], &(camera.pos), (char*)"Vine3_", 30, size));
                 }
                 else if(!strcmp(textureName, (char*)"textures/vine.jpg")){
                   int random = rand()%3;
                   if(random == 0){
                      strcpy(textureName, (char*)"Vine_");
                   }
                   else if(random == 1){
                      strcpy(textureName, (char*)"Vine2_");
                   }  
                   else if(random == 2){
                      strcpy(textureName, (char*)"Vine3_");
                   }
                   world.add(new obj2d(x, y, z, world.all[parentid], &(camera.pos), textureName, 30, size));
                 }
                 else{
                  world.add(new obj2d(x, y, z, world.all[parentid], &(camera.pos), LoadTexture(textureName), size, 1));
                 }
                 world.all2d[world.all2d.size()-1]->objectivenum = tempONum;
                 strcpy(world.all2d[world.all2d.size()-1]->objectivename, tempOName);
              }
           }
       }
     }
     fclose (map);
     for(unsigned int i = 0; i < triggers.size(); i++){
       triggers[i]->updateDetail = false;
     }
}

void Objective::update(float dt){
  int numHit = 0;
  //intro();
  for(unsigned int i = 0; i < triggers.size(); i++){
    if(triggers[i]->hit){
      numHit++;
    }
  }
  if(numHit > currentComplete){
    currentComplete = numHit;
    //check if collectible name = Download , for loop through triggers, if update detail, draw progress bar with numhit/10%
    updateDetail();
  }
}

void Objective::draw(float x,float y){
   char *ch;
   glColor4f(1,1,1,.5);
   if (currentComplete == totalNeeded) glColor4f(.5,.5,.5,.5);
   glRasterPos3f(GW*x, GH*y, 0.0);
   char dout[100];
   sprintf(dout,"- %s (%d/%d)", objectiveName, currentComplete, totalNeeded);
   for(ch= dout; *ch; ch++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*ch);
   }
   
}


