#include "importMap.h"

/*
 * This class will be used to import a .map file from the mapper tool
 * It will generate a vector of Objects that will represent the geometry
 * of the Lume world.
 *
 */

extern SData world;
extern Player character;
extern std::vector<Level> levels;
extern Level* currentLevel;

extern char curStage[128];
extern int currentobjectivenum;
extern char currentobjectivename[128];
extern pnt3d cinematicGoal; 
extern pnt3d character_start;
extern pnt3d objective_start;
extern vector<pnt3d*> cam_path;
extern trigger* freeform_cam;
const float SCALE = .5;

void importMap::import() {
  //cout << "loading: " << mapName << "\n";
  int px, py, pz;
  px = py = pz = 0;
  char mapbuffer[64];

  

  sprintf(mapbuffer, "maps/%s/map", mapName);

  FILE* map;
  map = fopen(mapbuffer, "r");
  if(map==NULL){
    //printf("File Not Found : %s\n",mapbuffer);
    return;
  }

  fclose (map);

  strcpy(curStage, mapName);
  std::vector<obj*> worldtemp;
  std::vector<obj2d*> world2dtemp;
  std::vector<Robot*> robottemp;
  std::vector<MovingObj*> movingtemp;
  std::vector<trigger*> triggers;
  float worldxmax = -1000000;
  float worldymax = -1000000;
  float worldzmax = -1000000;
  float worldxmin = 1000000;
  float worldymin = 1000000;
  float worldzmin = 1000000;
  bool levelExist = false;

  char buffer[1024];
  char* token;

  for(unsigned int i = 0; i < levels.size(); i++){
    if(!(strcmp(levels[i].levelName, mapName))){
      levelExist = true;
      currentLevel = &levels[i];
      currentLevel-> load();//do that init stuff
    }
  }

  if(!levelExist){
    //camera.intro = true;
    //camera.introStart = true;
    levels.push_back(Level());
    strcpy(levels[levels.size()-1].levelName, mapName);
    currentLevel = &levels[levels.size()-1];
    currentLevel->load();//do that init stuff
    currentLevel->levelComplete = false;//doesnt work in constructor....wtf?
    if(!strcmp(curStage, (char*)"overworld")){
      // intro shit //      
      //camera.intro = false;
      //camera.introStart = false;
      //camera.pathFinished = true;
      //cam_path.clear();
      // //
      currentLevel->displayObjectives = false;
    }
    else if(!strcmp(curStage, (char*)"tutorial")){
      currentLevel->displayObjectives = false;
    }
    else{
      currentLevel->displayObjectives = true;
      //camera.intro = true;
      //camera.introStart = true;
      //camera.pathFinished = false;
    }
     sprintf(mapbuffer, "maps/%s/objective", mapName);
     map = fopen(mapbuffer, "r");
     bool objective = true;
     if(map==NULL){
       //printf("No objective for: %s\n",mapName);
       objective = false;
     }

     if(objective){
        fgets (buffer , 1024 , map);
        while (!feof(map)){
          token = strtok(buffer, "\n");//totalNeeded
          int total = atoi(token);
          fgets (buffer , 1024 , map);
          token = strtok(buffer, "\n") ;//objectiveName
          char objName[128];
          strcpy(objName, token);
          fgets (buffer , 1024 , map);
          token = strtok(buffer, "\n") ;//collectableName
          char collectName[128];
          strcpy(collectName, token);
          fgets (buffer , 1024 , map);
          //create objective
          levels[levels.size()-1].objectives.push_back(new Objective(0, total, objName, collectName, triggers));
        }
        fclose (map);
     }

    if(strcmp(currentLevel->levelName, (char*)"suburbs") && strcmp(currentLevel->levelName, (char*)"overworld")){
      currentLevel->activeMovingPlatforms = true;
    }
  }
  //printf("%d\n",levels.size());
  if(!currentLevel->displayObjectives){
    currentLevel->displayObjectives = true;
    currentLevel->activeMovingPlatforms = true;
    for(unsigned int i = 0; i < levels.size(); i++){
      if(levels.size() != 6 || (strcmp(levels[i].levelName, (char*)"overworld") && !levels[i].levelComplete)){
        if(strcmp(currentLevel->levelName, "tutorial"))
          currentLevel->activeMovingPlatforms = false;
        currentLevel->displayObjectives = false;
        break;
      }
    }
    if(currentLevel->displayObjectives){
      hud->Insight.show("You can now reach KOG's fortress", 400);
      hud->Insight.show("It is time to take KOG down!", 400);
    }
  }


  sound.playLoop(currentLevel->music());
  sprintf(mapbuffer, "maps/%s/map", mapName);

  map = fopen(mapbuffer, "r");

  world.removeAll();//reset world

   


  fgets (buffer , 1024 , map);

  while (!feof(map)){
    token = strtok(buffer, " ");//x1
    float minx = atof(token);
    token = strtok(NULL, " ") ;//y1
    float miny = atof(token);
    token = strtok(NULL, " ");//z1
    float minz = atof(token);
    token = strtok(NULL, " ");//x2
    float maxx = atof(token);
    token = strtok(NULL, " ");//y2
    float maxy = atof(token);
    token = strtok(NULL, " ");//z2
    float maxz = atof(token);
    token = strtok(NULL, " ");//texture name
    char textureName[64];
    strcpy(textureName, token);
    token = strtok(NULL, "\n");//type
    char type[64];
    strcpy(type, token);
    fgets (buffer , 1024 , map);

    float dx = abs(maxx - minx) * SCALE;
    float dy = abs(maxy - miny) * SCALE;
    float dz = abs(maxz - minz) * SCALE;
    float x = minx * SCALE + .5 * dx;
    float y = miny * SCALE + .5 * dy;
    float z = minz * SCALE + .5 * dz;
    if(maxx > worldxmax){
      worldxmax = maxx;
    }
    if(maxy > worldymax){
      worldymax = maxy;
    }
    if(maxz > worldzmax){
      worldzmax = maxz;
    }
    if(minx < worldxmin){
      worldxmin = minx;
    }
    if(miny < worldymin){
      worldymin = miny;
    }
    if(minz < worldzmin){
      worldzmin = minz;
    }

    if(!strcmp(type, "Energy")){
      world2dtemp.push_back(new Source(x, y, z, .5, 4));
      currentLevel->totalEnergy+=4;
      sound.playLoop("energysource.ogg",x,y,z);
      worldtemp.push_back(new Tree(x+1.5, y-2.0, z-2.0));
      worldtemp.push_back(new Tree(x+2.0, y-2.0, z+1.2));
      worldtemp.push_back(new Tree(x-1.7, y-2.0, z+1.0));
    }
    else if(!strcmp(type, "LEnergy")){
      world2dtemp.push_back(new Source(x, y, z, 10, 50));
      currentLevel->totalEnergy+=50;
      sound.playLoop("energysource.ogg",x,y,z);
    }
    else if(!strcmp(type, "Ramp")){
      int rotate = 0;
      if(dx < dz){
         rotate = 1;
      }
      worldtemp.push_back(newRamp(x, y, z, dx, dy, dz, rotate, 0, textureName));
    }
    else if(!strcmp(type, "Ramp180")){
      int rotate = 0;
      if(dx < dz){
         rotate = 1;
      }
      worldtemp.push_back(newRamp(x, y, z, dx, dy, dz, rotate, 1, textureName));
    }
    else if(!strcmp(type, "Roof")){
      int rotate = 1;
      if(dx < dz){
         rotate = 0;
      }
      worldtemp.push_back(newRamp(x, y, z, dx, dy, dz, rotate, 0, textureName));
    }
    else if(!strcmp(type, "Roof180")){
      int rotate = 1;
      if(dx < dz){
         rotate = 0;
      }
      worldtemp.push_back(newRamp(x, y, z, dx, dy, dz, rotate, 1, textureName));
    }
    else if(!strcmp(type, "Arch")){
      //temp arch
      int size = 1;
      worldtemp.push_back(newCube(x, y, z - dz / 2 + (size * SCALE), dx, dy, (size * 2 * SCALE), textureName));
      worldtemp.push_back(newCube(x, y, z + dz / 2 - (size * SCALE), dx, dy, (size * 2 * SCALE), textureName));
      worldtemp.push_back(newCube(x, y + dy / 2 - (size * SCALE), z, dx, (size * 2 * SCALE), dz - (size * 4 * SCALE), textureName));
    }
    else if(!strcmp(type, "Fog")){
      world2dtemp.push_back(new obj2d(x,y,z,0,1,0,dx,dz,&(camera.pos),textureName));
      world2dtemp[world2dtemp.size() - 1]->classId = FOG_ID;
      //worldtemp.push_back(newCube(x, y, z, dx, dy, dz, textureName));
    }
   else if(!strcmp(type, "Ground")){
    	worldtemp.push_back(newCube(x, y, z, dx, dy, dz, textureName));
      worldtemp[worldtemp.size() - 1]->classId = GROUND_ID;
    }
    else if(!strcmp(type, "Spawn")){
      px = x;
      py = y;
      pz = z;
      character_start = pnt3d(x, y, z);
    }
    else if(!strcmp(type, "Sky")){
    	worldtemp.push_back(newCube(x, y, z, dx, dy, dz, textureName));
      worldtemp[worldtemp.size() - 1]->classId = SKY_ID;
    }
    else if(!strcmp(type, "Cube")){
    	worldtemp.push_back(newCube(x, y, z, dx, dy, dz, textureName));
    }
    else if(!strcmp(type, "Glass")){
      worldtemp.push_back(newCube(x, y, z, dx, dy, dz, NULL));
    }
    else{//teleport or collectible
      token = strtok(type, " ");
      if(!strcmp(token, "Trigger")){
         token = strtok(NULL, "\0");
         strcpy(type, token);
             trigger* trig = new trigger(x, y, z, dx, dy, dz, type);
             //worldtemp.push_back(trig);
             if(!levelExist){
               for(unsigned int i = 0; i < currentLevel->objectives.size(); i++){
                 if(strcmp(type, currentLevel->objectives[i]->collectableName) == 0){
                   currentLevel->objectives[i]->triggers.push_back(trig);
                   if(i == 0){
                     objective_start = pnt3d(trig->x, trig->y+0.5, trig->z);
                   }
		 }
               }
             }      
         if(!strcmp(type, (char*)"Console")){
            worldtemp.push_back(newCube(x, y-2, z, 1, 2, 1, (char*)"textures/concrete.jpg"));
            worldtemp.push_back(newCube(x, y-1, z, 2, .25, 2, (char*)"textures/console.jpg"));
            worldtemp[worldtemp.size() - 1]->classId = SKY_ID;
         }
      }
      else if(!strcmp(type, "Tele")){
         token = strtok(NULL, "\0");
         strcpy(type, token);
         bool active = false;
         //adding the arch
         worldtemp.push_back(newModel(x, y, z, dx, dy, dz, NULL, (char*)"models/arch/arch.obj", 0));
         //adding collision to arch
       	int size = 1;
       	dx = 5;
       	dy = 5.25;
       	dz = .75;
       	worldtemp.push_back(newCube(x-dx/2+(size*SCALE), y-1, z-.2, (size * 1.75 * SCALE), dy, dz, textureName));
       	worldtemp[worldtemp.size() - 1]->hidden = true;
         worldtemp.push_back(newCube(x+dx/2-(size*SCALE), y-1, z-.2, (size * 1.75 * SCALE), dy, dz, textureName));
         worldtemp[worldtemp.size() - 1]->hidden = true;
         worldtemp.push_back(newCube(x, y + dy / 2 - (size * SCALE) - 1, z -.2, dx - (size * 4 * SCALE), (size * 2 * SCALE), dz, textureName));
         worldtemp[worldtemp.size() - 1]->hidden = true;
         //adding the actual tele
         if(!strcmp(curStage, (char*)"overworld")){
            if (allLevels) {
               active = true;
            }
            else if(!strcmp(type, (char*)"ghetto")){
               active = true;//always unlocked
            }
            else if(levels.size() > 2 && !strcmp(type, (char*)"city") && levels[2].objectives[0]->isComplete()){
               active = true;//unlocked after finishing first objective in ghetto
            }
            else if(levels.size() > 3 && !strcmp(type, (char*)"suburbs") && levels[3].objectives[0]->isComplete()){
       	      active = true;//unlocked after visiting city
       	   }
       	   else if(levels.size() > 4 && !strcmp(type, (char*)"skyline") && levels[4].objectives[0]->isComplete()){
       	      active = true;//unlocked after finishing first objective in suburbs
       	   }
       	   else if(!strcmp(type, (char*)"KOG")){
       	      active = true;//always unlocked
       	   }
       	}
       	else{
       	   active = true;
       	}
       	worldtemp.push_back(newTele(x, y, z, dx, dy, dz, textureName, type, active));
       	if(active){
       	   worldtemp.push_back(newCube(x, y - 1, z, 3, 3, .5, textureName));
       	   worldtemp[worldtemp.size() - 1]->hidden = true;
       	}
    	}
    	else if(!strcmp(type, "Model")){
    	  token = strtok(NULL, " ");
    	  strcpy(type, token);
    	  token = strtok(NULL, "\0");
    	  int modelrotation = 0;
    	  if(!strcmp(token, (char*)"+x")){
    	    modelrotation = 0;
    	  }
    	  else if(!strcmp(token, (char*)"-z")){
    	    modelrotation = 1;
    	  }
    	  else if(!strcmp(token, (char*)"-x")){
    	    modelrotation = 2;
    	  }
    	  else if(!strcmp(token, (char*)"+z")){
    	    modelrotation = 3;
    	  }
    	  if(!strcmp(type, (char*) "models/monitor/monitor.obj")){
    	    worldtemp.push_back(newCube(x, y + 1, z, 1, 5, 4, (char*)"textures/concrete.jpg"));
    	    worldtemp[worldtemp.size() - 1]->hidden = true;
    	  }
    	  worldtemp.push_back(newModel(x, y, z, dx, dy, dz, textureName, type, modelrotation));
    	}
    	else{
    	  printf("unsupported type\n");
    	}
    }
  }
  fclose (map);

  sprintf(mapbuffer, "maps/%s/floaters", mapName);
  map = fopen(mapbuffer, "r");
  bool floaters = true;
  if(map==NULL){
    //printf("No floaters for: %s\n",mapName);
    floaters = false;
  }

  if(floaters){
     fgets (buffer , 1024 , map);
     while (!feof(map)){
       token = strtok(buffer, " ");//x
       float x = atof(token);
       token = strtok(NULL, " ");//y
       float y = atof(token);
       token = strtok(NULL, " ");//z
       float z = atof(token);
       pnt3d dir;
       token = strtok(NULL, " ");//dir x
       dir.x = atof(token);
       token = strtok(NULL, " ");//dir x
       dir.y = atof(token);
       token = strtok(NULL, " ");//dir x
       dir.z = atof(token);
       token = strtok(NULL, " ");//size(height == width)
       float size = atof(token);
       char type[64];
       strcpy(type, token);
       token = strtok(NULL, "\n");//texture name
       char textureName[64];
       strcpy(textureName, token);
       fgets (buffer , 1024 , map);

       world.addFloater(new obj2d(x, y, z, dir, &(camera.pos), LoadTexture(textureName), size));
     }
     fclose (map);
  }

  sprintf(mapbuffer, "maps/%s/detail", mapName);
  map = fopen(mapbuffer, "r");
  bool detail = true;
  if(map==NULL){
    //printf("No detail for: %s\n",mapName);
    detail = false;
  }

  if(detail){
     fgets (buffer , 1024 , map);
     while (!feof(map)){
       token = strtok(buffer, " ");//parent id
       int parentid = atoi(token);
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
       
       if(!strncmp(textureName, (char*)"textures/nyan/", 14)){
          world2dtemp.push_back(new obj2d(x, y, z, worldtemp[parentid], &(camera.pos), size));
          sound.playLoop("fpa.ogg",x,y,z);
       }
       /*else if(!strncmp(textureName, (char*)"textures/Vine_/", 15)){
          world2dtemp.push_back(new obj2d(x, y, z, worldtemp[parentid], &(camera.pos), (char*)"Vine_", 30, size));
       }
       else if(!strncmp(textureName, (char*)"textures/Vine2_/", 16)){
          world2dtemp.push_back(new obj2d(x, y, z, worldtemp[parentid], &(camera.pos), (char*)"Vine2_", 30, size));
       }
       else if(!strncmp(textureName, (char*)"textures/Vine3_/", 16)){
          world2dtemp.push_back(new obj2d(x, y, z, worldtemp[parentid], &(camera.pos), (char*)"Vine3_", 30, size));
       }
       else if(!strcmp(textureName, (char*)"textures/vine.jpg")){
         int random = rand()%3;
         if (random == 0) {
            strcpy(textureName, (char*)"Vine_");
         }
         else if (random == 1) {
            strcpy(textureName, (char*)"Vine2_");
         }  
         else if (random == 2) {
            strcpy(textureName, (char*)"Vine3_");
         }
         world2dtemp.push_back(new obj2d(x, y, z, worldtemp[parentid], &(camera.pos), textureName, 30, size));
       }*/
       else{
          world2dtemp.push_back(new obj2d(x, y, z, worldtemp[parentid], &(camera.pos), LoadTexture(textureName), size, 1));
       }
       world2dtemp[world2dtemp.size()-1]->objectivenum = tempONum;
       strcpy(world2dtemp[world2dtemp.size()-1]->objectivename, tempOName);
     }
     fclose (map);
  }

  sprintf(mapbuffer, "maps/%s/path", mapName);
  map = fopen(mapbuffer, "r");
  bool path = true;
  if(map==NULL){
    //printf("No path for: %s\n",mapName);
    path = false;
  }
  cam_path.clear();
  if(path){
     
     fgets (buffer , 1024 , map);
     while (!feof(map)){
       token = strtok(buffer, " ");

       if (!strcmp(token, "Robot")) {
         //printf("I AM ROBOT\n");
         token = strtok(NULL, " ");//id
         unsigned int id = atoi(token);
         token = strtok(NULL, " ") ;//x
         float x = atof(token);
         token = strtok(NULL, " ");//y
         float y = atof(token);
         token = strtok(NULL, "\n");//z
         float z = atof(token);
         fgets (buffer , 1024 , map);

         if(id == robottemp.size()){
           robottemp.push_back(new Robot(pnt3d(x,y,z)));
         }
         else{
           robottemp[id]->addNode(pnt3d(x,y,z));
         }
       }
       else if (!strcmp(token, "Moving")) {
         float dx = 0, dy = 0, dz = 0;
         bool control = 0;
         token = strtok(NULL, " ");//id
         unsigned int id = atoi(token);
         token = strtok(NULL, " X:") ;//x
         float x = atof(token);
         token = strtok(NULL, " Y:") ;//y
         float y = atof(token);
         token = strtok(NULL, " Z:");//z
         float z = atof(token);
         if (id == movingtemp.size()) {
            token = strtok(NULL, " DX:");//dx
            dx = atof(token);
            token = strtok(NULL, " DY:");//dy
            dy = atof(token);
            token = strtok(NULL, " DZ:");//dz
            dz = atof(token);
            token = strtok(NULL, " Control:");//control
            control = atoi(token);
         }
         token = strtok(NULL, " Speed:");//speed
         float speed = atof(token);
         token = strtok(NULL, "\n");//texture name
         char textureName[64];
         strcpy(textureName, token);
         fgets (buffer , 1024 , map);

         if (id == movingtemp.size()) {
           movingtemp.push_back(new MovingObj(pnt3d(x, y, z), dx, dy, dz, textureName, currentLevel->activeMovingPlatforms, speed, control));
         }
         else {
           movingtemp[id]->addNode(pnt3d(x, y, z), speed);
         }

       }
       else if (!strcmp(token, "Camera") && !levelExist) {
         //cout << "cam\n";
         token = strtok(NULL, " ") ;//x
         float x = atof(token);
         token = strtok(NULL, " ") ;//y
         float y = atof(token);
         token = strtok(NULL, "\n");//z
         float z = atof(token);
         pnt3d* path_section = new pnt3d(x, y, z);
         cam_path.push_back(path_section);
         fgets (buffer , 1024 , map);
       }
       else{
         fgets (buffer , 1024 , map);
       }
     }
     if(strcmp(curStage, (char*)"overworld") && !levelExist){
      // if not the overworld then load path for intro
       camera.initPath();
     }
     fclose (map);
  }


  //create sds
  worldxmax*=SCALE;
  worldzmax*=SCALE;
  worldxmin*=SCALE;
  worldzmin*=SCALE;
  float offsetx = worldxmin;
  float lengthx = worldxmax - worldxmin;
  float offsety = worldymin + 25;
  float lengthy = worldymax - worldymin;
  float ymid = offsety + (lengthy/2);
  float offsetz = worldzmin;
  float lengthz = worldzmax - worldzmin;
  world.resize(offsetx, offsetz, lengthx, lengthz);
  //printf("%f %f %f %f\n", offsetx, offsetz, lengthx, lengthz);
  //int n = rand()%worldtemp.size();
  //sound.playLoop("fpa.ogg",worldtemp[n]->x,worldtemp[n]->y,worldtemp[n]->z);
  if(!strcmp(mapName, "tutorial")){
    ymid = 75;
  }
  cinematicGoal = pnt3d(offsetx + (lengthx/2), (ymid < (worldymax - 20))?ymid:(worldymax - 20), offsetz + (lengthz/2));
    //test
   for(unsigned int i = 0; i < currentLevel->objectives.size(); i++){
     for(unsigned int j = 0; j < currentLevel->objectives[i]->triggers.size(); j++){
       worldtemp.push_back(currentLevel->objectives[i]->triggers[j]);
     }
   }
  
  if(currentLevel){
    if(currentLevel->objectives.size() > 0){
      if(currentLevel->objectives[0]->triggers.size() > 0){
        /*objective_start = pnt3d(currentLevel->objectives[0]->triggers[triggers.size()-1]->x, 
                                currentLevel->objectives[0]->triggers[triggers.size()-1]->y,
                                currentLevel->objectives[0]->triggers[triggers.size()-1]->z);*/
      }
    }
  }
  

  //add freeform_cam trigger
  world.add(freeform_cam);
  for(unsigned int i = 0; i < worldtemp.size(); i++){
      world.add(worldtemp[i]);
  }

  for(unsigned int i = 0; i < robottemp.size(); i++){
      world.addRobot(robottemp[i]);
  }

  for(unsigned int i = 0; i < movingtemp.size(); i++) {
      world.addMovingObj(movingtemp[i]);
  }
  

     


  for(unsigned int i = 0; i < world2dtemp.size(); i++){
      if(!strcmp(world2dtemp[i]->objectivename,(char*)"null"))
         world.add(world2dtemp[i]);
      else{
        //add level objective logic here based on currentlevel data
        for(unsigned int j = 0; j < currentLevel->objectives.size(); j++){
           //printf("%s : %s : %d %d\n",world2dtemp[i]->objectivename, currentLevel->objectives[j]->collectableName, currentLevel->objectives[j]->currentComplete, world2dtemp[i]->objectivenum);
           if(!strcmp(world2dtemp[i]->objectivename, currentLevel->objectives[j]->collectableName)){
             if(currentLevel->objectives[j]->currentComplete >= world2dtemp[i]->objectivenum){
                world.add(world2dtemp[i]);
             }
           }
        }
      }
  }

  //world.createNet();

  character.setCheckpoint(px, py, pz);
  character.x = px;
  character.y = py;
  character.z = pz;
  
  if(!levelExist){
    if(!strcmp(curStage, (char*)"overworld")){
      hud->Insight.show("Welcome to the City's central teleport hub.", 400);
      hud->Insight.show("You can reach different parts of the city through these teleporters.", 400);
      hud->Insight.show("In order to reach new areas you must collect more of KOG's energy and breach KOG's security.",600);
    }
    else if(!strcmp(curStage, (char*)"ghetto")){
      hud->Insight.show("Welcome to the slums.", 400);
      hud->Insight.show("Each district has its own energy source which keeps it under the control of KOG.", 600);
      hud->Insight.show("Reach the top of the district to reclaim the town's energy from KOG.", 600);
      hud->Insight.show("KOG robots are surveying the city, so you must be careful to not run into them.", 600);
      hud->Insight.show("I suggest sprinting or building to stop them.", 600);
    }
    else if(!strcmp(curStage, (char*)"city")){
      hud->Insight.show("Welcome to the commerial district.", 400);
      hud->Insight.show("As you have probably noticed KOG now controls everything from fast food to night clubs.", 600);
      hud->Insight.show("This district's energy source is located on top of the KOG Tower.", 600);
    }
    else if(!strcmp(curStage, (char*)"suburbs")){
      hud->Insight.show("Welcome to the suburbs.", 400);
      hud->Insight.show("You must take back the district's energy and steal KOG's encryption keys.", 600);
      hud->Insight.show("I can activate the grav lifts to help you reach the keys after you steal KOG's energy.", 600);
    }
    else if(!strcmp(curStage, (char*)"skyline")){
      hud->Insight.show("Welcome to skyline.", 400);
      hud->Insight.show("This is KOG's main energy hub.", 600);
      hud->Insight.show("KOG will not let you obtain the sources easily.", 600);
      hud->Insight.show("Aquire all the energy sources and you will face KOG!", 600);
    }
  }
}

void importMap::createFloaters(){
   FILE* floatersMap;
   char mapbuffer[64];
   sprintf(mapbuffer, "maps/%s/floaters", mapName);
   floatersMap = fopen(mapbuffer, "w");
   char buffer[1024];

   for(unsigned int i = 0; i < world.all2d.size(); i++){
      if(world.all2d[i]->classId == FLOATER2D_ID){//if not a 2d sprite created by a growing object
         sprintf(buffer, "%f %f %f %f %f %f %f %s", world.all2d[i]->getX(), world.all2d[i]->getY(), world.all2d[i]->getZ(),
               world.all2d[i]->getNorm().x, world.all2d[i]->getNorm().y,
               world.all2d[i]->getNorm().z, world.all2d[i]->getSize(), getTexName(world.all2d[i]->getTexId()));
         fputs(buffer, floatersMap);
         fputs("\n", floatersMap);
      }
   }

   fclose(floatersMap);
}

void importMap::createDetail(){
   FILE* detailMap;
   char mapbuffer[64];
   sprintf(mapbuffer, "maps/%s/detail", mapName);
   detailMap = fopen(mapbuffer, "w");
   char buffer[1024];

   for(unsigned int i = 0; i < world.all2d.size(); i++){
   	//printf("%d\n", world.all2d[i]->getTexId());
      if(world.all2d[i]->classId == DETAIL2D_ID){//if not a 2d sprite created by a growing object
         sprintf(buffer, "%d %f %f %f %f %s %s %d", world.all2d[i]->getParentI(), world.all2d[i]->getX(), world.all2d[i]->getY(), world.all2d[i]->getZ(), world.all2d[i]->getSize(), getTexName(world.all2d[i]->getTexId()), world.all2d[i]->objectivename, world.all2d[i]->objectivenum);
         fputs(buffer, detailMap);
         fputs("\n", detailMap);
      }
   }

   fclose(detailMap);
}
