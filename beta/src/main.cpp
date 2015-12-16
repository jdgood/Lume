#include "util.h"
#include "obj.h"
#include "tex.h"
#include "gobj.h"
#include "sound.h"
#include "shader.h"
#include "player.h"
#include "cam.h"
#include "importMap.h"
#include "robot.h"
#include "orb.h"
#include "model.h"
#include "hud.h"
#include "debug.h"
#include "sprite.h"
#include "frustum.h"
#include "shadow.h"
#include "sdata.h"
#include "gameState.h"
#include "billboard2d.h"
#include "level.h"
#include "script.h"
#include "tree.h"
#include "FBO.h"
#include "particle.h"
#include <sys/resource.h>

#define GRAVACC .01 //gravital acceleration
#define GRAVTERM 0.6 //terminal velocity

#define ACC 0.2 //deceleration

//window globals
int GW;
int GH;
int gx, gy;

//world
cam camera;
float lastx, lasty;//mouse dragg
float fps;
float grav;//player y velocity

//player movement
bool jumpActive;
bool isBuilding;
bool isRemoving;
bool gridmode;
bool allLevels =  false;
int dirx;
int dirz;

float acc = 1.0;
long startLoop;
float gwh;
float highscore;
float highscoreSave;
bool finished = false;
bool gameStart = false;
bool editmode = false;
int edittex = 0;
float editsize = 2;
pnt3d editdir = pnt3d(1, 0, 0);
pnt3d editoffset = pnt3d(0, 0, 0);
pnt3d sf = pnt3d(1,1,1);
bool gameLoading = true;
int spriteLoader = 1;
int ufo = 0;
bool fullScreen = false;
char curStage[128];
int mapModTime;
int glowc= 0;

bool canLoad = false;
bool fullLoad = false;
bool intro = false;
bool outro = false;
int introLength = 0;
int outroLength = 0;
int supd;

//keyboard
bool keystates[256] = {false};

//enviroment
bool bPause;
bool wireframe = false;
bool basic = false;
bool bShadows = false;
bool nochar = false;
bool load = true;
bool models = true;
bool glow = true;
bool BLUR_ON = true;
bool BLOOM_ON = false;
bool frustumMode = false;

//message triggers
bool blockvirgin = true;

void reshape(int w, int h);
void gameLoop(int n);
void loadLoop(int n);
void menuLoop(int n);
void introLoop(int n);
void outroLoop(int n);
void startGame();
void display();

//define frustum
frustum frustum;

//list of levels
std::vector<Level> levels;

//level variables
Level* currentLevel;
pnt3d edgeColor;
float waveEffect[8];
float glowBrightness;

//transition effect counter and player pos
float transitionCounter = 0;
bool resetview = false;
pnt3d cinematicGoal;
pnt3d cinematicCurrent;

//config variables
float mouseSensitivity = 1;

//detail map
int currentobjectivenum = 0;
char currentobjectivename[128];

//CAMERA GLOBALSDESUDESUDESUDESUDESUDESU
pnt3d character_start;
pnt3d objective_start;
vector<pnt3d*> cam_path;
bool freeform = false;
trigger* freeform_cam = new trigger(15, 30, 15, 0, 0, 0, (char *)"freeform_cam");
//optimizations
int lightsourcetexture;

irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
SoundFX sound = SoundFX(engine);
Shader* shader;
Shader* bloom;
//Shader* pixeld;
FBO* fbo;
Player character;// = Player();
debug* debugger = new debug();
HUD* hud;
shadow shadows = shadow(0,0,0);
SData world = SData();

void loadStage(int stage){
   char curStageFile[128];
   struct stat buf;

   sprintf(curStage, "lume%d", stage);
   importMap(curStage).import();

   sprintf(curStageFile, "maps/%s/map", curStage);
   FATALERROR(stat(curStageFile, &buf) == -1, "stat");
   mapModTime = buf.st_mtime;
}

void loadStage(char* stage){
	char curStageFile[128];
   struct stat buf;

   importMap(stage).import();
   //for (int i = 0 ; i< 1000; i++){
   //   world.add(new sentinel());
   //}

   sprintf(curStageFile, "maps/%s/map", stage);
   FATALERROR(stat(curStageFile, &buf) == -1, "stat");
   mapModTime = buf.st_mtime;
}

void loadOverworld(){
   char curStageFile[128];
   struct stat buf;

   sprintf(curStage, "overworld");
   importMap(curStage).import();

   sprintf(curStageFile, "maps/%s/map", curStage);
   FATALERROR(stat(curStageFile, &buf) == -1, "stat");
   mapModTime = buf.st_mtime;
}

void constructScene(){
   char curStageFile[128];
   struct stat buf;

   sprintf(curStage, "tutorial");
   importMap(curStage).import();
   //for (int i = 0 ; i< 1000; i++){
   //   world.add(new sentinel());
   //}   

   sprintf(curStageFile, "maps/%s/map", curStage);
   FATALERROR(stat(curStageFile, &buf) == -1, "stat");
   mapModTime = buf.st_mtime;
}

void drawScene(){
   glMatrixMode(GL_MODELVIEW);

   glPushMatrix();
      camera.setup();
      glLoadName(-2);

      if (!nochar) character.draw();

      frustum.updateMatrix(camera);

      std::vector<drawable*> objs;
      //objs.push_back(newCube(6,4,10,1000,1000,1000));
      
      if (gridmode){
         world.draw();
         objs = world.getObjsViewable(character.x,character.y,character.z);
      } else {
         //if(!strcmp(curStage, (char*)"overworld")){
         //   objs = world.getView();
         //}
         //else{
            objs = world.getView(frustum);//,character.x,character.y,character.z);//normal draw
         //}
      }
      if (basic) {
         glLineWidth(5);
         wireframe = true;
         for (unsigned int i = 0; i < objs.size(); i++){
            if(objs[i]->classId != 2) {
               objs[i]->draw();
            }
         }
         wireframe = false;
         glLineWidth(1);
      }
      //glUseProgram(shader->frag);
      //int loc = glGetUniformLocation(shader->frag, "fbo_tex");

      //glActiveTexture(shader->fbo.texid);	
      //glBindTexture(GL_TEXTURE_2D, shader->fbo.texid);

      //glUniform1i(loc, shader->fbo.texid);
      // bloom->bind();

      for (unsigned int i = 0; i < objs.size(); i++){
         if(objs[i]->classId != ORB_ID && objs[i]->classId != GLASS_ID) {
            //if (objs[i]->index == 24){printf("tree %d\n",rand()%10);}
            objs[i]->draw();
         }
      }
  // bloom->unbind();
      //glUseProgram(0);

      // make sure orbs on top
      for (unsigned int i = 0; i < objs.size(); i++){
         if(objs[i]->classId == ORB_ID) {
              objs[i]->draw();
			     Source * temp = (Source*)objs[i];
              glDisable (GL_DEPTH_TEST);
			     temp->drawOrbs();
              glEnable (GL_DEPTH_TEST);
		   }
      }
    
      if (!gridmode && bShadows){
         shader->bind();
         std::vector<obj*> sobjs;
         sobjs = world.getShadowObjs(frustum);
         for (unsigned int i = 0; i < sobjs.size(); i++){
            shadows.draw(sobjs[i]);
         }
         shader->unbind();
      }
   glPopMatrix();
}

void drawGlow(){
   //glEnable(GL_CULL_FACE);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
      basic = true;
      models = false;
      camera.setup();
      glLoadName(-2); 
      frustum.updateMatrix(camera);

      if (frustumMode) { 
         glLoadIdentity();
         gluLookAt(0,50,0,//camera.pos.px, camera.pos.py - 20, camera.pos.pz, //eye position
            camera.pos.px,camera.pos.py,camera.pos.pz, //reference point
            0.0, 1.0, 0.0);
      }
      
      if (!nochar) character.draw();
      
      std::vector<obj*> objs;

      objs = world.getViewObjs(frustum);//,character.x,character.y,character.z);//normal draw

      for (unsigned int i = 0; i < objs.size(); i++){
         if (objs[i]->classId != SKY_ID)objs[i]->draw();
      }

      glLineWidth(7);
      glBindTexture(GL_TEXTURE_2D, -1);
      wireframe = true;
      for (unsigned int i = 0; i < objs.size(); i++){
         if (objs[i]->classId != SKY_ID)objs[i]->draw();
      }
      wireframe = false;
      glLineWidth(1);

      /*if (!gridmode && bShadows){
         shader->bind();
         std::vector<obj*> sobjs;
         sobjs = world.getShadowObjs(frustum);
         for (unsigned int i = 0; i < sobjs.size(); i++){
            shadows.draw(sobjs[i]);
         }
         shader->unbind();
      }*/
      
      models = true;
      basic = false;
   glPopMatrix();
   //glDisable(GL_CULL_FACE);
}


void loadAllTextures() {
   DIR *dir;
   DIR *sprite_dir;
   struct dirent *d;
   struct dirent *sprite_d;
   struct stat info;

   FATALERROR(chdir("textures") == -1, "chdir");
   dir = opendir(".");
   FATALERROR(dir == NULL, "opendir");

   while (true) { /* inside textures dir */
      display();
      char dd[128];
      d = readdir(dir);
      if (d != NULL) {
         sprintf(dd, "textures/%s", d->d_name);

         FATALERROR(lstat(d->d_name, &info) == -1, d->d_name);

         if (!(strncmp(d->d_name, ".", 1) 
                  && strcmp(d->d_name, "tron_skybox") && strcmp(d->d_name, "sky")
                  && strcmp(d->d_name, "greenworld_skybox"))) {
            ;//skip
         }
         else if (S_ISREG(info.st_mode)) { /* if .jpg load */
            /* must be in main dir to load */
            FATALERROR(chdir("..") == -1 , "chdir");

            LoadTexture(dd);

            /* move back to textures dir */
            FATALERROR(chdir("textures") == -1 , "chdir");
         }
         else if (S_ISDIR(info.st_mode) && (!strcmp(d->d_name, "menu") ||
               !strcmp(d->d_name, "intro") || !strcmp(d->d_name, "Outro_") ||
               !strncmp(d->d_name, "growth", 6) ||
               !strncmp(d->d_name, "Vine", 4))) { /* if dir go into */
            char dir_name[128];
            strcpy(dir_name, d->d_name);

            FATALERROR(chdir(d->d_name) == -1 , d->d_name);
            sprite_dir = opendir(".");
            FATALERROR(sprite_dir == NULL, "opendir");

            int i = 0;
            while (true) { /* inside sprite dir */
               sprite_d = readdir(sprite_dir);
               if (sprite_d != NULL){
                  if (!(strncmp(sprite_d->d_name, ".", 1))) {
                     ;//skip
                  }
                  else { /* if .jpg load */
                     char texture[128];
                     sprintf(texture, "textures/%s/%s%03d.jpg", 
                           dir_name, dir_name, i); 
                     i++;
      
                     /* must be in main dir to load */
                     FATALERROR(chdir("../..") == -1, "chdir");

                     display();
                     LoadTexture(texture);
                     
                     /* move back to textures dir */
                     FATALERROR(chdir(dd) == -1, "chdir");
                  }
               }
               else { /* leaving Sprite dir */
                  FATALERROR(chdir("..") == -1, "chdir");
                  break;
               }
            }
         }
      }
      else { /* done traversing dir */
         break;
      }
   }
   FATALERROR(chdir("..") == -1, "chdir");
   gameLoading = false;
   display();
}

void display() {
   //glow pass
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->frame);
   glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
   glLoadIdentity();
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gwh = ((float)GW)/GH;
   gluPerspective(60,((float)GW)/GH, 0.05, 2000);
   //bloom->bind();
   if(!basic && !wireframe && glow) drawGlow();
   //bloom->unbind();


   //normal pass
   glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gwh = ((float)GW)/GH;
   gluPerspective(60,((float)GW)/GH, 0.05, 2000);

   if (character.loadingStage == 0 && hud->Insight.visible == false) {
      //loadStage(character.stage);
      if (!strcmp(currentLevel->levelName, (char*)"KOG")) {
         sound.playLoop("lumecred.ogg");
         outro = true;
         hud->outrotex.frame = 1;
         for(unsigned int i = 0; i < levels.size(); i++){
           if(!strcmp(levels[i].levelName, (char*)"overworld")){
             levels[i].levelComplete = true;
             levels[i].objectives[0]->currentComplete = 1;
           }
         }
      }
      hud->Insight.clear();
      loadOverworld();
      character.loadingStage = -1;
      camera.cinematic = false;
      camera.lookAt(&character.x,&character.y,&character.z);
      camera.reset();
   }
   else if (character.loadingStage > 0) {
      character.loadingStage--;
   }

   if (gameLoading) {
      if (fullLoad) { 
         fbo->draw();
         hud->drawLoadScreen();
         hud->update(4);
      
         if (load) {
            load = false;
            loadAllTextures();
            hud->loadStart();
         }
      }
      else {
         gameLoading = false;
         glutPostRedisplay();
      }
   } 
   else if(intro){
     fbo->draw();
     hud->drawIntro();
     glutTimerFunc(TIMERINT, introLoop, TIMERINT);
     if((int)hud->introtex.indexes.back() == hud->introtex.id){
       intro = false;
       startGame();
     }
   }
   else if(outro){
     fbo->draw();
     hud->drawOutro();
     glutTimerFunc(TIMERINT, outroLoop, TIMERINT);
     if ((int)hud->outrotex.indexes[75] == hud->outrotex.id) {
        hud->outrotex.play(300);
     }
     if((int)hud->outrotex.indexes.back() == hud->outrotex.id){
        outro = false;
        fbo->update(1000);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        bPause = true;
        sound.playLoop("Lume.ogg");
     }
   }
   else{
      drawScene();
      debugger->draw();
      fbo->draw();

      if (character.gameStarted && !bPause) {
        hud->draw();
      }
      else {
         glutTimerFunc(TIMERINT, menuLoop, TIMERINT);
         hud->introtex.frame = 0;
         hud->drawMainMenu(gx, gy);
      }
   }
   
   
   glutSwapBuffers();

   //sound.update(0);
   if (supd > 2) {sound.update(0); supd = 0;}
   supd++;
}

std::vector<int> selector(float x,float y){//uses picking to find object at x,y (mouse coord)
 	GLuint buff[64] = {0};
 	GLint hits, view[4];
 	glSelectBuffer(64, buff);//set buffer
 	glRenderMode(GL_SELECT);//enter select mode
 	
   glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
   
   glGetIntegerv(GL_VIEWPORT,view);
	gluPickMatrix(x,view[3]-y,
			.1,.1,view);
	gluPerspective(60, ((float)GW)/GH, 0.1, 40);

   glInitNames();//start names
 	glPushName(0);//make sure not empty
 	
   drawScene();//draw and name
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   hits = glRenderMode(GL_RENDER);//get hits and change back to render

   std::vector<int> objs;
   int j = (int)hits;
   j--;
   while (j >= 0){
      int tmp = (int)(buff[j*4 + 3]);
      if (tmp >= 0 && tmp < world.size()) {
         bool here = false;         
         for (unsigned int loop = 0; loop < objs.size(); loop++){
            if (tmp == objs[loop]) here = true;
         }
         if (!here) objs.push_back(tmp);
      }
      j--;
   }
   /*printf("hit:");
   for (int i = 0; i < objs.size(); i++){
      printf("%d,",objs[i]);
   }
   printf("\n");*/
   return objs;
}

void reshape(int w, int h) {//reshape window
	GW = w;
	GH = h;
   glViewport(0, 0, GW, GH);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
 
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();

   fbo->resize();
   hud->Insight.resize();
}

void keyboardUp(unsigned char key, int x, int y ){
   switch( key ) {
   case 'e' : case 'E' :
      glutIgnoreKeyRepeat(0);
      fbo->blurAmount = 1;
      fbo->blurOffset = 0;
      character.isSprinting = false;
      character.sprintEnergy = 0;
      character.canSprint = true;
      keystates['e'] = false;
      keystates['E'] = false;
     break;
   case 'a' : case 'A' :
     keystates['a'] = false;
     keystates['A'] = false;
     break;  
   case 'w' : case 'W' :
     keystates['w'] = false;
     keystates['W'] = false;
     break;
   case 's' : case 'S' :
     keystates['s'] = false;
     keystates['S'] = false;
     break;
   case 'd' : case 'D' ://glutPassiveMotionFunc(NULL);
     keystates['d'] = false;
     keystates['D'] = false;
     break;
   case ' ' : 
     keystates[' '] = false;
     break;
   case 'z' : case 'Z' :
     keystates['z'] = false;
     keystates['Z'] = false;
     break;
   }
   keystates[key] = false;
}

void startGame() {
   glutSetCursor(GLUT_CURSOR_NONE);
   glutWarpPointer(GW / 2, GH / 2);
   
   character = Player();   
   
   constructScene();
   character.gameStarted = true;
   hud = new HUD();
   bPause = false;

   camera = cam(&(character.x),&(character.y),&(character.z));

   character.gameStarted = true;
   glutTimerFunc(TIMERINT, gameLoop, TIMERINT);
}

void keyboard(unsigned char key, int x, int y ){
   if (debugger-> active){
      debugger->type(key);
      return;
   }
   std::vector<obj*> objs = world.getAllObjs();
   switch( key ) {
      case 13:
         if (hud->Insight.enabled) {
            printf("Insight logic here\n");
         }
         break;
      case '1':
         gameState().save((char*)"saves/game1"); 
         gameState().load((char*)"saves/game1");
         break;
      case 27: //brings up the game menu
         if(bPause && character.gameStarted){
            startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
            glutWarpPointer(GW / 2, GH / 2);
            glutTimerFunc(TIMERINT, gameLoop, TIMERINT);
            glutSetCursor(GLUT_CURSOR_NONE);
            bPause = false;
            if (currentLevel) sound.playLoop(currentLevel->music());
         }
         else if(intro){
            //printf("skipping intro\n");
            hud->introtex.id = (int)hud->introtex.indexes.back();
         }
         else if(outro){
            //printf("skipping outro\n");
            hud->outrotex.id = (int)hud->outrotex.indexes.back();
         }
         else if(camera.cinematic){
            if(strcmp(currentLevel->levelName, (char*)"KOG")){
               hud->Insight.clear();
               loadOverworld();
               character.loadingStage = -1;
               camera.cinematic = false;
               camera.lookAt(&character.x,&character.y,&character.z);
               camera.reset();
            }
         }
         else if(camera.intro){
            cam_path.clear();
            camera.pathFinished = true;
            camera.intro = false;
            camera.lookAt(&character.x,&character.y,&character.z);
            camera.reset();
         }
         else if (character.gameStarted) {
            fbo->update(1000);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            bPause = true;
            sound.playLoop("Lume.ogg");
         }
         break;
      case 'r': case 'R' :
         character.x = character.checkpoint.x;//reset point
         character.y = character.checkpoint.y +1;
         character.z = character.checkpoint.z;
         break;
      case 't': case 'T' :
         //fbo->blurAmount++;
         sf.x = sf.y = sf.z = sf.z + .1;
         break;
      case 'g': case 'G' :
         sf.x = sf.y = sf.z = sf.z - .1;
         //fbo->blurAmount--;
         break;
      case 'y': case 'Y' :
         //fbo->blurIntensity +=.05;
         break;
      case 'h': case 'H' :
         //fbo->blurIntensity -=.05;
         break;
      case 'u': case 'U' :
         //fbo->blurOffset += 0.001;
         break;
      case 'j': case 'J' :
         //fbo->blurOffset -= 0.001;
         break;
      case 'i': case 'I' :
         //fbo->blurDecay += 0.001;
         break;
      case 'k': case 'K' :
         //fbo->blurDecay -= 0.001;
         break;
      case 'f': case 'F':

         if (!fullScreen) {
            glutFullScreen();
            fullScreen = true;
         }
         else {
            glutReshapeWindow(GW, GH);
            fullScreen = false;
         }
         break;
      case 'q' :case 'Q':
         debugger->visible = !debugger->visible;
         break;
      case 'z': case 'Z' :
         //camera.cinematic = !camera.cinematic;
         //camera.cine = 0;
         //camera.offset = 2; 
         
         //new function freeform camera stuff
          
         freeform = !freeform;
         if(freeform){
           character.control(freeform_cam);
         }
         else{
           character.releaseControl();
         } 
         break;
      case 'v': case 'V' :
         if(freeform){
           freeform_cam->y += 0.1 * character.level;
         }
         break;
      case 'b' : case 'B' :
         if(freeform){
           freeform_cam->y -= 0.1 * character.level;
         }
      case 'x': case 'X' :
         editmode = false;
         break;
      case ']': case '}' :
         world.add(new ParticleSystem(pnt3d(character.x, character.y,
                    character.z), 100, (char *)"textures/lightsource.jpg"));
         break;

      case '[': case '{' :
         world.add(new obj2d(character.x + editoffset.x,
                 character.y + editoffset.y,character.z + editoffset.z,
                 editdir, &(camera.pos),edittex,editsize));
         break;
      case '-': case '_' :      
         character.addEnergy(-1);
         break;
      case '=': case '+':
         character.addEnergy(1);
         break;
      case '9': case '(' :
         camera.zoom(.9);         
         break;
      case '0': case ')':
         camera.zoom(1.1);
         break;
      case '`': case '~':
         debugger-> active = true;
         break;
   }
   //printf("pressed %c\n", key);
   keystates[key] = true;
} 

float p2wx(int xp) {
   return 2 * ((float)(2*GW*xp) / (GH*(GW-1)) - ((float)GW / GH));
}

float p2wy(int yp) {
   return 2 * (((float)(2*yp) / (GH-1)) - 1);
}

int g2py(int yp) {
   return (GH-1) - yp;
}

void hudClick(int x, int y){
   switch (hud->drawMainMenu(x, y)) {
      case SAVE_GAME :
         gameState().save((char*)"saves/game1"); 
      case RESUME :
         startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
         glutWarpPointer(GW / 2, GH / 2);
         glutTimerFunc(TIMERINT, gameLoop, TIMERINT);
         glutSetCursor(GLUT_CURSOR_NONE);
         bPause = false;
         if (currentLevel) sound.playLoop(currentLevel->music());
         break;      
      case NEW_GAME :
         levels.clear();
         currentLevel = NULL;
         hud->introtex.frame = 1;
         intro = true;
         camera.introStart = false;
         cam_path.clear();
         camera.pathFinished = true;
         introLength = 0;
         //startGame();//uncommenting this allows new game to work again is there a reason this was commented out?
         break;
      case LOAD_GAME :
         //startGame();
         if (fullLoad) {
            hud->loadStart();
         }
         if(!gameState().load((char*)"saves/game1")){
            break;
         }
         startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
         //hud = new HUD();
         glutWarpPointer(GW / 2, GH / 2);
         glutTimerFunc(TIMERINT, gameLoop, TIMERINT);
         glutSetCursor(GLUT_CURSOR_NONE);
         bPause = false;
         break;
      case EXIT :
         glutLeaveGameMode();
         exit( EXIT_SUCCESS );
         break;
   }
}

void mouseClick(obj* o, pnt3d d){
   if(character.curenergy > 0 || editmode){
      if (o->classId == GOBJ_ID){
         isBuilding = true;
         character.frameCount = 0;
         o->click(d.x,d.y,d.z);              
         return;
      } 
      else if(o->classId == RAMP_ID){
         hud->Insight.show("Insight: You cannot build on ramps.");
         return;
      } else if (o->classId == MOVINGOBJ_ID || o->classId == ROB_ID) {
         o->click(d.x,d.y,d.z);
         return;
      } 
      else {
         o->click(d.x,d.y,d.z);
      }
      o->texture.play();
      d.x+=camera.pos.x/(40 * camera.pos.mag());
      d.y+=camera.pos.y/(40 * camera.pos.mag());
      d.z+=camera.pos.z/(40 * camera.pos.mag());
      //printf("Point: %f %f %f",d.x,d.y,d.z);
      d.x = (float)(round(d.x*2))/2;//snap to grid
      d.y = (float)(round(d.y*2))/2;
      d.z = (float)(round(d.z*2))/2;
      pnt3d dir = o->getNorm(d.x,d.y,d.z);
      //printf("Point: %f %f %f Dir: %f %f %f\n",d.x,d.y,d.z,dir.x,dir.y,dir.z);
      if (editmode){
         if(edittex == (int)LoadTexture((char*)"textures/nyan/nyan000.jpg")){
            world.add(new obj2d(d.x,d.y,d.z,o,&(camera.pos), editsize));
         }
         else{
            world.add(new obj2d(d.x,d.y,d.z,o,&(camera.pos),edittex,editsize,1));
         }
         world.all2d[world.all2d.size()-1]->objectivenum = currentobjectivenum;
         strcpy(world.all2d[world.all2d.size()-1]->objectivename, currentobjectivename);
         return;
      }        
      sound.play("grow.wav",d.x,d.y,d.z);//"grow.wav"
      if (blockvirgin){ }//hud->Insight.show("Insight: You build your first block!"); blockvirgin = false;}
      isBuilding = true;
      character.frameCount = 0;
      currentLevel->blocksBuilt++;
      world.add(newGrowingCube(d.x,d.y,d.z,dir.x,dir.y,dir.z));//make cube
      world.add(new obj2d(d.x,d.y,d.z,o,&(camera.pos)));//,&(hud->sprinttex)));//make cube
      world.addShadow(world.all[world.size()-1]);
      //printf("shadows: %d\n",world.sobj.size());
      character.addEnergy(-1);
      character.addExp(4);
      character.dir.x = -camera.pos.x;
      character.dir.z = -camera.pos.z;
   }

}

void mouseRightClick(obj* o, pnt3d d){
   if (editmode){
      world.all2d.pop_back();
      return;
   }
   if (!o) return;
   if (o->classId == GOBJ_ID){//if growing obj
      isRemoving = true;
      character.frameCount = 0;
      currentLevel->blocksBuilt--;
      sound.play("ungrow.ogg",d.x,d.y,d.z);
      o->classId = -1;
      character.addEnergy(1);
      character.addExp(-3.5);            
      return;
   }
}

void mouse(int button, int state, int x, int y) {
   if (state == GLUT_DOWN) { // if the left button is clicked 
      //used for navigating the menu    
      if(!character.gameStarted || bPause) {
         if (button == GLUT_LEFT_BUTTON && !intro) hudClick(x,y);
      } else {
         if (character.target){character.releaseControl(); return;}//release control of mobj        
         std::vector<int> hits = selector(GW/2,GH/2);//get object built upon
         if (hits.size() <= 0) return; 
         bool hit = false;
         int hitobj = -1; 
         pnt3d d = pnt3d(camera.pos.px,camera.pos.py,camera.pos.pz);        
         while((!hit) && (d.x/camera.pos.x + d.y/camera.pos.y + d.z/camera.pos.z > -1000)){//loop til hit
            for (unsigned int i = 0; i < hits.size(); i++){                
               if ((world.get(hits[i])) && world.get(hits[i])->hitTest(d.x,d.y,d.z)){
                  hit = true;
                  hitobj = hits[i];
               }
            }               
            d.x-=camera.pos.x/(40 * camera.pos.mag());
            d.y-=camera.pos.y/(40 * camera.pos.mag());
            d.z-=camera.pos.z/(40 * camera.pos.mag());
         }
         if (hit == false) return;
         obj* o = world.get(hitobj);
         if (button == GLUT_LEFT_BUTTON) {
            mouseClick(o,d);
         } else {
            mouseRightClick(o,d);
         }            
      }
   }
}

void keyactions(){
  if (fbo->state == 2) return;

  if(keystates['a'] || keystates['A']){
    dirx = 1;
  }
  
  if(keystates['d'] || keystates['D']){
    dirx = -1;
  }
  
  if(!(keystates['a'] || keystates['A']) && !(keystates['d'] || keystates['D'])){
    dirx = 0;
  }
  
  if(keystates['w'] || keystates['W']){
    dirz = -1;
  }
  
  if(keystates['s'] || keystates['S']){
    dirz = 1;
  }
  
  if(!(keystates['s'] || keystates['S']) && !(keystates['w'] || keystates['W'])){
    dirz = 0;
  }
  
  if(keystates[' ']){
    if(jumpActive){
      grav = -0.05;
      character.isJumping = true;
      jumpActive = false;
    }
  }
  else if(!keystates[' ']){
    character.isJumping = false;
  }
  
  if(keystates['e'] || keystates['E']){
     if (character.canSprint) {
       glutIgnoreKeyRepeat(1);
       character.isSprinting = true;
       fbo->blurAmount = 3;
       fbo->blurOffset = 0.011;
       dirz = -3;
     }
     else {
       fbo->blurAmount = 1;
       fbo->blurOffset = 0;
     }
  }
}

void mouseMove(int x, int y) {
   if (!bPause) {
      camera.update(mouseSensitivity * (p2wx(x) - lastx), mouseSensitivity * (p2wy(g2py(y)) - lasty));
   }
   lastx = p2wx(x);
   lasty = p2wy(g2py(y));

   if (!character.gameStarted || bPause) {
      gx = x;
      gy = y;
      glutPostRedisplay();
   }
}

void activeMouseMove(int x, int y) {
   if (!bPause) {
      camera.update(mouseSensitivity * (p2wx(x) - lastx), mouseSensitivity * (p2wy(g2py(y)) - lasty));
   }
   lastx = p2wx(x);
   lasty = p2wy(g2py(y));

   if (!character.gameStarted || bPause) {
      gx = x;
      gy = y;
      glutPostRedisplay();
   }
}

void gravity(float dt,std::vector<obj*> objs){
   // Player gravity //
   character.y-=grav*dt;//add velocity from gravity
   if(character.isJumping){ //if space is held down
       grav -= (10*character.maxjump+grav)*dt/200;// increase jump accel
       if(-grav >= character.maxjump){ //max jump vel reached
         character.isJumping = false;
       }
   }
   else{
     grav+=GRAVACC*dt;//gravity acceleration
   }
   if (grav > GRAVTERM) grav = GRAVTERM;
   // Fall off edge //
   if (character.y < -11){
     sound.play("woosh.wav");
     character.respawn();
     grav = 0;
     return;
  }

   // Movement decceleration //
   if (((dirx)*(dirx) + (dirz)*(dirz) == 0)) {
      character.walkacc = 0;
      if (jumpActive){ 
         character.vel.x -= character.vel.x * ACC * dt;
         character.vel.z -= character.vel.z * ACC * dt;
      } else { // if in the air
         character.vel.x -= character.vel.x * ACC * dt/4;
         character.vel.z -= character.vel.z * ACC * dt/4;
      }
   }
  
  // Object collisions //
  for (unsigned int i = 0; i < objs.size(); i++){ //loop through all obj in area
   // Jumping up into object //
   if(objs[i]->classId != GOBJ_ID || character.level < 10){ //if gobj, allow user to jump through block 
      if (grav <= 0){
         if (objs[i]->hitCollision(character)){// your head is in an object!
            while (objs[i]->hitTest(character.x,character.y+character.ymax+0.1,character.z)){
               character.y-=.01;
            }
            grav = .1;
            return;
         }
      }
   } else {
      if (objs[i]->hitCollision(character)){
         //if (grav <= 0){
            objs[i]->visible = false;
         //}
      } else {
         objs[i]->visible = true;
      }
   }
   // Standing on object //
   if (objs[i]->hitCollision(character)){
      //printf("classid: %d objid: %d\n", objs[i]->classId, i);
		if (objs[i]->classId != ROB_ID){//if not robot 
         if (objs[i]->classId != GROUND_ID)
            objs[i]->fullColor();
         //objs[i]->wireFrameBrightness = 1;
         if(objs[i]->classId == GOBJ_ID && character.level >= 10){ //for gobj, you must jump through most of it before you pop through
            float tmp = character.y; 
            character.ymax /= 10;
            character.fy /= 10;
            while ((tmp-character.y)*(tmp-character.y) < .3*dt*dt && (objs[i]->hitCollision(character))){
              //(objs[i]->hitTest(character.x,character.y+character.ymin,character.z) || objs[i]->hitTest(character.x,character.y,character.z))){
               character.y+=.01;
            }
            character.ymax *= 10;
            character.fy *= 10;
            if (tmp != character.y){
               objs[i]->visible = true;
               grav = .1;
               character.isJumping = false;
               jumpActive = true;
            }
         } else { //normal standing on obj
            while (objs[i]->hitCollision(character)){
               character.y+=.01;
            }          
            grav = .1;
            character.isJumping = false;
            jumpActive = true;
         }
      }
      else {//If object i is a robot...
         if (dirz == -3) { //if sprinting
            currentLevel->robotsKilled++;
            sound.play("explo.ogg",objs[i]->x,objs[i]->y,objs[i]->z);
            for (unsigned int j = 0; j < world.all.size(); j++) {
               if (world.all[j]->index == objs[i]->index) {
                  world.all.push_back(new ParticleSystem(pnt3d(character.x, character.y, character.z), 50));
                  world.all.erase(world.all.begin()+j);
               }
            }
            for (unsigned int j = 0; j < world.robs.size(); j++) {
               if (world.robs[j]->index == objs[i]->index) {
                  world.robs.erase(world.robs.begin()+j);
               }
            }
         }
      }
   }
   // camera collsions
   //camera.zoom(1.01);
   //int loop = 0;
   if(objs[i]->classId != ROB_ID && !camera.cinematic){
      //printf("%d\n", objs[i]->classId); 
      if(objs[i]->hitCamera(camera.pos.px + camera.pos.x,camera.pos.py + camera.pos.y-.1,camera.pos.pz + camera.pos.z)){
         while(objs[i]->hitCamera(camera.pos.px + camera.pos.x,camera.pos.py + camera.pos.y-.1,camera.pos.pz + camera.pos.z) && camera.zoomlevel < 1000){
            camera.zoomin();
        }
      }
      else{
         camera.zoomout(*objs[i]);
      }
   }
   //camera.zoom(0.99);
 }//for loop end
 //if (camera.pos.mag() < 5.385) camera.zoom(1.05);//reset to default
 
 /* teleporter and effects */
 //replace world with a list of teleporter only objects in importmap maybe? have a global do teleport flag?
 bool teleporting = false;
 for (unsigned int i = 0; i < world.all.size(); i++){
   if(world.all[i]->classId == TELEPORT_ID && world.all[i]->doTeleport()){
      if (!transitionCounter){
         sound.MusicState = -1; //fade out music
         //fbo->blurAmount = 10;
      }
      //fbo->blurOffset = .03;
      if((int)transitionCounter % 2 == 0){
         fbo->blurOffset += .003;
         fbo->blurAmount++;//necessary?
      }
      transitionCounter+=dt;
      
      if(fbo->blurAmount == 20){
         world.all[i]->setLoading();
         loadStage(world.all[i]->getLevelName());
         resetview = true;
      }
      teleporting = true;
      break;
   }
 }
 if(!teleporting){
   resetview = true;
 }
 if(resetview){
   if((int)transitionCounter % 2 == 0){
     fbo->blurOffset -= .003;
     fbo->blurAmount--;
   }
   transitionCounter-=dt;
   if(fbo->blurAmount <= 1){
     fbo->blurAmount = 1;
     fbo->blurOffset = 0;
     transitionCounter = 0;
     resetview = false;
   }
 }
   
}

void menuLoop(int n){
   long oldStartLoop = startLoop;
   startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
   float timeElapsed = (float) (startLoop - oldStartLoop);
   timeElapsed/=TIMERINT;
   hud->update(timeElapsed);
   glutPostRedisplay();
   if (bPause) glutTimerFunc(TIMERINT, menuLoop, TIMERINT);
}

void introLoop(int n){
   long oldStartLoop = startLoop;
   startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
   float timeElapsed = (float) (startLoop - oldStartLoop);
   timeElapsed/=TIMERINT;
   hud->update(timeElapsed);
   introLength++;
   glutPostRedisplay();
   if (bPause) glutTimerFunc(TIMERINT, introLoop, TIMERINT);
}

void outroLoop(int n){
   long oldStartLoop = startLoop;
   startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
   float timeElapsed = (float) (startLoop - oldStartLoop);
   timeElapsed/=TIMERINT;
   hud->update(timeElapsed);
   outroLength++;
   glutPostRedisplay();
   if (bPause) glutTimerFunc(TIMERINT, outroLoop, TIMERINT);
}

void gameLoop(int n){
   keyactions();
   long oldStartLoop = startLoop;
   startLoop = (long) glutGet(GLUT_ELAPSED_TIME);
   float timeElapsed = (float) (startLoop - oldStartLoop);
   fps = 1000/timeElapsed;
   timeElapsed/=TIMERINT;
   glutWarpPointer(GW / 2, GH / 2);//set pointer to middle of screen
   lastx = p2wx(GW / 2);// allow for view movement
   lasty = p2wy(g2py(GH / 2));

   /*if(currentLevel){
     if(currentLevel->objectives.size() > 0){
       if(currentLevel->objectives[0]->triggers.size() > 0){
         objective_start = pnt3d(currentLevel->objectives[0]->triggers[currentLevel->objectives[0]->triggers.size()-1]->x, currentLevel->objectives[0]->triggers[currentLevel->objectives[0]->triggers.size()-1]->y, currentLevel->objectives[0]->triggers[currentLevel->objectives[0]->triggers.size()-1]->z);
         //cout << "HI " << objective_start.x << " " << objective_start.y << " " << objective_start.z << "\n";
       }
     }
   }   */
   if (timeElapsed > 15)
      timeElapsed = 1;

   camera.update(0,0);//so holding mouse down wont effect camera
   if (dirx || dirz) character.setVel(character.maxspeed*dirx,character.maxspeed*dirz);//apply velocity

   //check collisions with only characters gridspace
   std::vector<obj*> objs = world.getObjs(character.x,character.y,character.z);//getAllObjs();
   currentLevel->update(timeElapsed);


   character.update(timeElapsed, objs);//move character x,z
   //sound.update(timeElapsed);
   gravity(timeElapsed,objs);//apply gravity y

   //update all objects
   objs = world.getAllObjs();
   for (unsigned int i = 0; i < objs.size(); i++){
      objs[i]->update(timeElapsed);
   }
   
   //update all sprites
   for (unsigned int i = 0; i < world.all2d.size(); i++){
      world.all2d[i]->update(timeElapsed);
   }

   //update all objectives
   /*if(currentLevel){
     cout << "objectives: " << currentLevel->objectives.size()-1 << "\n";
     for(unsigned int j = 0; j < currentLevel->objectives.size(); j++){
       currentLevel->objectives[j]->update(timeElapsed);
     }
   }*/
   
   fbo->update(timeElapsed);//update blur if needed
   hud->update(timeElapsed);
   if (!finished) {
      highscore += timeElapsed;
      if (character.y > 70) {
         finished = true;
         if (highscore < highscoreSave) {
            FILE *fp;
            fp = fopen("highscore.txt", "w");
            fprintf(fp, "%f", highscore);
            fclose(fp);
         }
      }
   }
   
   if (camera.intro || camera.cinematic) {
      //glutKeyboardFunc(NULL);
      //glutMouseFunc(NULL);
      if (camera.cinematic) {
         //glutMotionFunc(NULL);
         //glutPassiveMotionFunc(NULL);
      }
      //printf("NULL\n");
   }
   else {
      //glutKeyboardFunc(keyboard);
      glutMouseFunc(mouse);
      glutMotionFunc(activeMouseMove);
      glutPassiveMotionFunc(mouseMove);
      //printf("NOT NULL\n");
   }
   
   char curStageFile[128];
   struct stat buf;
   sprintf(curStageFile, "maps/%s/map", curStage);
   FATALERROR(stat(curStageFile, &buf) == -1, "stat");
   //printf("%s: %d : %d\n", curStageFile, (int)buf.st_mtime, (int)mapModTime);
   if (buf.st_mtime != mapModTime) { 
      gameState().save((char*)"saves/game1"); 
      gameState().load((char*)"saves/game1");
      mapModTime = buf.st_mtime;
   }
   glutPostRedisplay();
   if (!bPause) glutTimerFunc(TIMERINT, gameLoop, TIMERINT);
}

void readConfig(){
  char buffer[1024];
  char* token;
  char key[128];
  float value;
  FILE* config;
  config = fopen((char*)"config.cfg", "r");
  fgets (buffer , 1024 , config);
  while (!feof(config)){
    token = strtok(buffer, " ");
    strcpy(key, token);
    token = strtok(NULL, " ");
    value = atof(token);
    fgets (buffer , 1024 , config);
    
    if(!strcmp("sensitivity", key)){
      mouseSensitivity = value;
    }
    else if(!strcmp("load-full", key)){
      if(value == 1){
        fullLoad = true;
      }
    }
    else if(!strcmp("bloom", key)){
      if(value == 1){
        BLUR_ON = false;
        BLOOM_ON = true;
      }
    }
    else if(!strcmp("fullscreen", key)){
      if(value == 1){
        glutFullScreen();
        fullScreen = true;
      }
    }
    else{
      printf("Unsupported config key: %s\n", key);
    }
  }
  fclose (config);
}

int main( int argc, char** argv ) {
  //set up my window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
  GW = 1080;GH= 720;
  glutInitWindowSize(GW, GH); 
  glutInitWindowPosition(50, 50);
  glutCreateWindow("Lume Beta");
  glClearColor(0.0, 0.0, 0.0, 1.0);
  
  //read in configuration file
  readConfig();
  
  glutMouseFunc( mouse );
  glutMotionFunc( activeMouseMove );
  glutPassiveMotionFunc( mouseMove );//** use glutMotionFunc for windows**
  glutDisplayFunc( display );

  glutReshapeFunc( reshape );
  glutKeyboardFunc(keyboard);
  glutKeyboardUpFunc(keyboardUp);
  //glutIgnoreKeyRepeat(1);
  
  glClearDepth(1.0f);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LINE_SMOOTH);

  //initialization
  grav = .02;
  dirx = 0;
  dirz = 0;
  camera = cam(&(character.x),&(character.y),&(character.z));//init cam
   glewInit();
   if (glewIsSupported("GL_VERSION_2_0"))
	   printf("Ready for OpenGL 2.0\n");
   else {
	   printf("OpenGL 2.0 not supported\n");
	   exit(1);
   } 
   
   // load in image textures
   /* Preloads all textures inside textures dir */
   if (argc > 1) {
      if(!strcmp(argv[1], "ufo")){
         ufo = 1;
      }
      else{
         fullLoad = true;
      }
   }
   LoadTexture((char*)"textures/concrete.jpg");
   character = Player(0,0,0); 
   hud = new HUD();
   canLoad = true;
   //hud->drawMainMenu(0, 0);//create a loading splash later???
   //glutSwapBuffers();
   fbo = new FBO();
   shader = new Shader(fbo->texid, fbo->texid2);
   bloom = new Shader(fbo->texid, fbo->texid2);
   //pixeld = new Shader(fbo->texid2, fbo->texid);
   LoadExternTexture((char *)"Glow");
   LoadExternTexture((char *)"FrameBuffer");
   lightsourcetexture = LoadTexture((char*)"textures/lightsource.jpg");
   //LoadExternTexture((char *)"3rdBuffer");
   //LoadTexture((char*)"textures/we_are_watching_dark.jpg");
   //loadAllTextures();

   FILE *fp;
   fp = fopen("highscore.txt", "r");
   fscanf(fp, "%f", &highscoreSave);
   fclose(fp);
   
   strcpy(currentobjectivename, (char*)"null");

   shader->loadFrag("shaders/shadow.frag");
   bloom->loadFrag("shaders/bloom.frag");
   //pixeld->loadFrag("shaders/pixeld.frag");

   const rlim_t kStackSize = 32 * 1024 * 1024;
   struct rlimit rl;
   int result;

   result = getrlimit(RLIMIT_STACK, &rl);
   if (result == 0) {
      if (rl.rlim_cur < kStackSize) {
         rl.rlim_cur = kStackSize;
         result = setrlimit(RLIMIT_STACK, &rl);
         if (result != 0)
            fprintf(stderr, "setrlimit returned %d", result);
      }
   }

   debugger->add("FPS",&fps);
   debugger->add("Player",&character);
   debugger->add("Camera",&(camera.pos));
   debugger->add("Level",&(character.level));
   debugger->add("Exp",&(character.exp));
   debugger->add("ExpNeeded",&(character.expNeeded));
   debugger->add("Energy",&(character.curenergy));
   debugger->add("ExpMax",&(character.totalenergy));
   debugger->add("Sprint",&(character.sprintEnergy));
   debugger->add("WINNING TIME",&(highscore));
   debugger->add("HIGHSCORE",&(highscoreSave));
   debugger->add("cine",&(camera.cine));
   debugger->add("offset",&(camera.offset));
   sound.playLoop("Lume.ogg");
   glutMainLoop();
}
