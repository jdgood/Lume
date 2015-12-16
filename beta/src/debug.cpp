#include "debug.h"

void saveConfig(){
  FILE* config;
  config = fopen((char*)"config.cfg", "w");
  char buffer[1024];
  
  sprintf(buffer, "sensitivity %f\n", mouseSensitivity);
  fputs(buffer, config);
  sprintf(buffer, "load-full %d\n", fullLoad?1:0);
  fputs(buffer, config);
  sprintf(buffer, "bloom %d\n", (BLOOM_ON && !BLUR_ON)?1:0);
  fputs(buffer, config);
  sprintf(buffer, "fullscreen %d\n", fullScreen?1:0);
  fputs(buffer, config);

  fclose(config);
}

void debug::draw(){
  char str[60];
  char *ch;
 glMatrixMode(GL_PROJECTION);
 glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, 1.0, 0.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
     glLoadIdentity();
     glBindTexture(GL_TEXTURE_2D, -1);
     //glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */
       glColor4f(0, 1, 0, 1);
      if (visible){      
         int j = 0;
         for (unsigned int i = 0; i < floats.size(); i++){
            sprintf(str, "%s: %3.2f\n", floats[i].name,*floats[i].data);
            glRasterPos3f(0.05, 0.95 - .02*i, 0.0);
            j++;
            for(ch= str; *ch; ch++) {
             glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
            }
         }
         for (unsigned int i = 0; i < ints.size(); i++){
            sprintf(str, "%s: %d\n", ints[i].name,*ints[i].data);
            glRasterPos3f(0.05, 0.95 - .02*j, 0.0);
            j++;
            for(ch= str; *ch; ch++) {
             glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
            }
         }
         for (unsigned int i = 0; i < objs.size(); i++){
            sprintf(str, "%s: %3.2f %3.2f %3.2f\n", objs[i].name,objs[i].data->x,objs[i].data->y,objs[i].data->z);
            glRasterPos3f(0.2, 0.95 - .02*i, 0.0);
            for(ch= str; *ch; ch++) {
             glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
            }
         }
         for (unsigned int i = 0; i < pnts.size(); i++){
            sprintf(str, "%s: %3.2f %3.2f %3.2f\n", pnts[i].name,pnts[i].data->x,pnts[i].data->y,pnts[i].data->z);
            glRasterPos3f(0.4, 0.95 - .02*i, 0.0);
            for(ch= str; *ch; ch++) {
             glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
            }
         }
         for (unsigned int i = 0; i < vecs.size(); i++){
            sprintf(str, "%s: %3.2f %3.2f %3.2f @ [%3.2f %3.2f %3.2f]\n", vecs[i].name,vecs[i].data->x,vecs[i].data->y,vecs[i].data->z,
                                                               vecs[i].data->px,vecs[i].data->py,vecs[i].data->pz);
            glRasterPos3f(0.6, 0.95 - .02*i, 0.0);
            for(ch= str; *ch; ch++) {
             glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
            }
         }
      }  
      if (active) { // draw input
         glRasterPos3f(0.4, 0.5, 0.0);
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 91);
         for(ch= input; *ch; ch++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
         }
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, 93);

         for (unsigned int i = 0; i < output.size(); i++){
            glColor4f(output[i].color.r, output[i].color.g, output[i].color.b, 1); 
            glRasterPos3f(0.4 + 0.1*(int(i/20)), 0.48 -.02*(i%20), 0.0);
            for(ch= output[i].text; *ch; ch++) {
               glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
            }
         }
      }
     //glPopAttrib();
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
 glPopMatrix();
}

void debug::add(const char* n,int* i){
   ints.push_back(intvar(n,i));
}

void debug::add(const char* n,float* f){
   floats.push_back(floatvar(n,f));
}

void debug::add(const char* n,pnt3d* p){
   pnts.push_back(pntvar(n,p));
}

void debug::add(const char* n,vec3d* v){
   vecs.push_back(vecvar(n,v));
}

void debug::add(const char* n,obj* o){
   objs.push_back(objvar(n,o));
}

void debug::addOut(const char* n){
   output.push_back(out(n));
}

void debug::addOut(const char* n, int c){
   out o = out(n);
   switch(c) {
      case 1 :
         o.color.r = 1; 
         o.color.g = 0;
         o.color.b = 0; 
         break;  
      case 2 :
         o.color.r = 0; 
         o.color.g = 0;
         o.color.b = 1; 
         break;
      default:
         o.color.r = 0; 
         o.color.g = 1;
         o.color.b = 0; 
         break;
   } 
   output.push_back(o);
}

void debug::clearInput(){
   while (len > 0){
      len--;
      input[len]= 0;
   }
}

void debug::type(char key){
   //printf("%d\n",key);
   switch( key ) {
      case 27://escape
         clearInput();
         output.clear();
         active = false;
         break;
      case '`': case '~':
         clearInput();
         output.clear();
         active = false;
         break;
      case 13://enter
         output.clear();
         command();
         clearInput();
         if (output.size() <= 0) active = false;
         break;
      case 8://backspace
         if (len > 0) len--;
         input[len]= 0;
         break;
      default:
         input[len] = key;
         len++;
         if (len > 60) len = 60;
   }
}

void debug::command(){
   char * token;
   if (len <= 0) return;
   token = strtok(input, " ");
   if (!token) return;
   if (strcmp ("exit",token)==0) { exit( EXIT_SUCCESS );
   }else if (strcmp ("rave",token)==0) {
      fbo->blurAmount = 10;
      fbo->blurIntensity = 5;
      fbo->blurOffset = 0.01;
   }else if (strcmp ("grid",token)==0) {
      gridmode = !gridmode;
   }else if (strcmp ("texture",token)==0) {
      char tex[64];
      token = strtok(NULL, " ");
      if (!token) {
         addOut("Invalid Texture!",1);
         addOut("Textures:",2);
         search("textures",".jpg");
         return;
      }
      sprintf(tex, "textures/%s.jpg", token);
      edittex = LoadTexture((char*)tex);
      editmode = true;
   }else if (strcmp ("tex",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      edittex = (int)atof(token);
      editmode = true;
   }else if (strcmp ("size",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      float e = atof(token);
      editsize = e;
      editmode = true;
   }else if (strcmp ("dir",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      editdir.x = (atof(token));
      token = strtok(NULL, " ");
      if (!token) return;
      editdir.y = (atof(token));
      token = strtok(NULL, " ");
      if (!token) return;
      editdir.z = (atof(token));
      editmode = true;
   }else if (strcmp ("offset",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      editoffset.x = (atof(token));
      token = strtok(NULL, " ");
      if (!token) return;
      editoffset.y = (atof(token));
      token = strtok(NULL, " ");
      if (!token) return;
      editoffset.z = (atof(token));
      editmode = true;
   }else if (strcmp ("insight",token)==0) {
      hud->Insight.enabled = !hud->Insight.enabled;
      hud->Insight.show("Insight: Hello!");
   }else if (strcmp ("edit",token)==0) {
      editmode = !editmode;
   }else if (strcmp ("allLevels",token)==0) {
      allLevels = !allLevels;
   }else if (strcmp ("wire",token)==0) {
      wireframe = !wireframe;
   }else if (strcmp ("basic",token)==0) {
      basic = !basic;
   }else if (strcmp ("frustum",token)==0) {
      frustumMode = !frustumMode;
   }else if (strcmp ("glow",token)==0) {
      glow = !glow;
   }else if (strcmp ("blur",token)==0) {
         BLUR_ON = !BLUR_ON;
   }else if (strcmp ("bloom",token)==0) {
         BLOOM_ON = !BLOOM_ON;
   }else if (strcmp ("shadows",token)==0) {
      bShadows = !bShadows;
   }else if (strcmp ("wwssadadba",token)==0) {
      character.setLevel(100);
      character.addEnergy(100);
   }else if (strcmp ("level",token)==0 || strcmp ("lvl",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      int e = (int)atof(token);
      character.setLevel(e);
   }else if (strcmp ("detail",token)==0) {
      importMap(curStage).createDetail();
   }else if (strcmp ("objective",token)==0){
      token = strtok(NULL, " ");
      if (!token) return;
      strcpy(currentobjectivename, token);
   }else if (strcmp ("objectivenum",token)==0){
      token = strtok(NULL, " ");
      if (!token) return;
      currentobjectivenum = atoi(token);
   }else if (strcmp ("floaters",token)==0) {
      importMap(curStage).createFloaters();
   }else if (strcmp ("exp",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      int e = (int)atof(token);
      character.addExp(e);
   }else if (strcmp ("energy",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      int e = (int)atof(token);
      character.addEnergy(e);
   }else if (strcmp ("control",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      unsigned int e = (int)atof(token);
      if (e < world.all.size()) character.control(world.all[e]);
   }else if (strcmp ("goto",token)==0) {
      token = strtok(NULL, " ");
      if (!token) return;
      float x = atof(token);
      token = strtok(NULL, " ");
      if (!token) return;
      float y = atof(token);
      token = strtok(NULL, " ");
      if (!token) return;
      float z = atof(token);
      character.x = x;
      character.y = y;
      character.z = z;
   }else if (strcmp ("map",token)==0) {
      char mapname[64];
      token = strtok(NULL, " ");
      if (!token) {
         addOut("Invalid Map!",1);
         addOut("Maps:",2);
         search("./maps",NULL,".",true);
         return;
      }
      sprintf(mapname, "%s", token);
      importMap(mapname).import();
   }else if(!strcmp("saveconfig",token)){
      saveConfig();
   }else if(!strcmp("sensitivity",token)){
      token = strtok(NULL, " ");
      if (!token) return;
      float temp = atof(token);
      mouseSensitivity = (temp > 0)?temp:mouseSensitivity;
   }else if(strcmp ("thereisacowlevel",token)==0){
     currentLevel->energyCollected = currentLevel->totalEnergy;
     for(unsigned int i = 0; i < currentLevel->objectives.size(); i++){
       for(unsigned int j = 0; j < currentLevel->objectives[i]->triggers.size(); j++){
         currentLevel->objectives[i]->triggers[j]->hit = true;
       }
     }
   }else if (strcmp ("help",token)==0) {
      addOut("Commands:",2);
      addOut("basic (toggle)");
      addOut("bloom (toggle)");
      addOut("blur (toggle)");
      addOut("control [int]");
      addOut("detail");
      addOut("edit (toggle)");
      addOut("energy [int]");
      addOut("exp [int]");
      addOut("glow (toggle)");
      addOut("goto [x] [y] [z]");
      addOut("grid (toggle)");
      addOut("lvl [int]");
      addOut("map [string]");
      addOut("saveconfig");
      addOut("sensitivity [n>0]");
      addOut("shadows (toggle)");
      addOut("size [float]");
      addOut("tex [int]");
      addOut("texture [string]");
      addOut("wire (toggle)");
      addOut("thereisacowlevel");
   }else {
      addOut("Invalid Command!",1);
   }
}

void debug::search(const char * dir){
   search(dir,NULL,NULL,false);
}

void debug::search(const char * dir, const char * filter){
   search(dir,filter,NULL,false);
}

void debug::search(const char * dir, const char * filter, const char * defilter){
   search(dir,filter,defilter,false);
}

//adds to output all files in @dir that contian @filter but not @defilter. If (@direct) then must be a directory. 
void debug::search(const char * dir, const char * filter, const char * defilter,bool direct){
   DIR *dd;
   struct dirent *ent;
   struct stat info;
   dd = opendir (dir);
   if (dd == NULL) return;
   while ((ent = readdir (dd)) != NULL) {
      char path[64];
      strcpy(path, dir);
      strcat(path, "/");
      strcat(path, ent->d_name);
      if ((filter) && (strstr(ent->d_name,filter))) {
         if ((defilter) && (strstr(ent->d_name,defilter))) {
         } else {
             if (!direct || ( (lstat(path, &info)>=0) && (info.st_mode&S_IFDIR) ))addOut(ent->d_name);
         }
      } else if (!filter){
         if ((defilter) && (strstr(ent->d_name,defilter))) {
         } else {
             if (!direct || ( (lstat(path, &info)>=0) && (info.st_mode&S_IFDIR) ))addOut(ent->d_name);
         }
      }
   }
   closedir (dd);
}

