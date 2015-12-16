#include "hud.h"

HUD::HUD(){
   energyPixels = 50;
   expPixels = 50;
   barstex = LoadTexture((char*)"textures/bars.jpg");
   bluebar = LoadTexture((char*)"textures/bluebar.jpg");
   orangebar = LoadTexture((char*)"textures/orangebar.jpg");
   download.id = 0;
   if (fullLoad) {
      loadtex.load((char*)"loading", 20);
      if (canLoad) {
         loadStart();
      }
   }
   else {
      loadtex.load((char*)"loading", 2);
      menutex.load((char*)"menu", 2);
      introtex.load((char*)"intro", 2);
      outrotex.load((char*)"Outro_", 2);
   }
   curlvl = 1;
   end_flag = false;
}

void HUD::loadMenu() {
   menutex.load((char*)"menu", 300);
}

void HUD::loadStart() {
   menutex.load((char*)"menu", 300);
   introtex.load((char*)"intro", 450);
   outrotex.load((char*)"Outro_", 165);
}

void HUD::draw() {
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0, GW, 0, GH);
   glScalef(1, -1, 1);
   glTranslatef(0, -GH, 0);
   glMatrixMode(GL_MODELVIEW);

   glEnable (GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
   glBindTexture(GL_TEXTURE_2D, -1);
   glColor4f(0.0, 0.0, 0.0, 0.4);
   glBegin(GL_POLYGON);
   glTexCoord2f(1,1);
   glVertex3f(0, GH*.125, -0.2);
   glTexCoord2f(0,1);
   glVertex3f(GW, GH*.125, -0.2);
   glTexCoord2f(0,0);
   glVertex3f(GW, 0, -0.2);
   glTexCoord2f(1,0);
   glVertex3f(0, 0, -0.2);
   glEnd();   

   Insight.drawBack();//must be here(before blendfunc changes)   

   /*glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
   glBindTexture(GL_TEXTURE_2D, sprinttex.id);
   glBegin(GL_POLYGON);
   glTexCoord2f(0,0);
   glVertex3f(GW-65, 15, -0.1);
   glTexCoord2f(1,0);
   glVertex3f(GW-5, 15, -0.1);
   glTexCoord2f(1,1);
   glVertex3f(GW-5, 75, -0.1);
   glTexCoord2f(0,1);
   glVertex3f(GW-65, 75, -0.1);
   glEnd();*/

   //HUD Background
   glColor3f(1.0, 1.0, 1.0);
   glBindTexture(GL_TEXTURE_2D, barstex);
   glBegin(GL_POLYGON);
   glTexCoord2f(1,1);
   glVertex3f(92.5, 15, -0.1);
   glTexCoord2f(0,1);
   glVertex3f(GW-92.5, 15, -0.1);
   glTexCoord2f(0,0);
   glVertex3f(GW-92.5, 75, -0.1);
   glTexCoord2f(1,0);
   glVertex3f(92.5, 75, -0.1);
   glEnd();
  
   //Energy   
   int temp = (character.curenergy * ((GW-200)/character.totalenergy) + 100)/HUD_PIXEL_RATE;
   temp *= HUD_PIXEL_RATE;

   if (character.curenergy > 0) {
      if ((int) energyPixels != temp) 
         (int) energyPixels > temp ? energyPixels-=HUD_PIXEL_RATE : energyPixels+=HUD_PIXEL_RATE;
   }
   else 
      energyPixels = 100;
   

   glBindTexture(GL_TEXTURE_2D, bluebar);
   glBegin(GL_POLYGON);
   glTexCoord2f(1,1);
   glVertex2f(100, 20);
   glTexCoord2f(0,1);
   glVertex2f(energyPixels, 20);
   glTexCoord2f(0,0);
   glVertex2f(energyPixels, 40);
   glTexCoord2f(1,0);
   glVertex2f(100, 40);
   glEnd();   
   

   //Experience
   temp = (int) (character.exp * ((GW-200) /(float) character.expNeeded) + 100)/HUD_PIXEL_RATE;
   temp *= HUD_PIXEL_RATE;
   
   if (character.exp > 0) {
      if ((int) expPixels != temp) 
         (int) expPixels > temp ? expPixels-=HUD_PIXEL_RATE : expPixels+=HUD_PIXEL_RATE;
   }
   else 
      expPixels = 100;
   
   glBindTexture(GL_TEXTURE_2D, orangebar);
   glBegin(GL_POLYGON);
   glTexCoord2f(1,1);
   glVertex2f(100, 50);
   glTexCoord2f(0,1);
   glVertex2f(expPixels, 50);
   glTexCoord2f(0,0);
   glVertex2f(expPixels, 70);
   glTexCoord2f(1,0);
   glVertex2f(100, 70);
   glEnd();

   if (download.id) {
      glBindTexture(GL_TEXTURE_2D, download.id);
      glBegin(GL_POLYGON);
      glTexCoord2f(1, 1);
      glVertex2f(GW/2 + 200, GH);
      glTexCoord2f(0, 1);
      glVertex2f(GW/2 - 200, GH);
      glTexCoord2f(0, 0);
      glVertex2f(GW/2 - 200, GH - 100);
      glTexCoord2f(1, 0);
      glVertex2f(GW/2 + 200, GH - 100);
      glEnd();
   }

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   //level ups
   char *ch;
   for (unsigned int i = 0; i < lvlups.size(); i++){
         glColor4f(lvlups[i].color[0],lvlups[i].color[1], lvlups[i].color[2],lvlups[i].color[3]);
         glRasterPos3f(GW*lvlups[i].x, GH*lvlups[i].y, 0.0);
         for(ch= lvlups[i].str; *ch; ch++) {
          glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)*ch);
         }
   }
  
   // doesn't show until menu is gone
   if (character.gameStarted) {
      //cross
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINES);
      glVertex2f(GW/2 -10, GH/2);
      glVertex2f(GW/2 -4, GH/2);
      glVertex2f(GW/2 +3, GH/2);
      glVertex2f(GW/2 +9, GH/2);

      glVertex2f(GW/2, GH/2 -10);
      glVertex2f(GW/2, GH/2 -4);
      glVertex2f(GW/2, GH/2 +4);
      glVertex2f(GW/2, GH/2 +10);
      glEnd();

      //level
      char levelNum [15];
      glColor3f(1.0, 1.0, 1.0);
      if(character.level > 9){
        glRasterPos3f(37, 47, 0.0);
      }
      else{
        glRasterPos3f(43, 47, 0.0);
      }
      sprintf(levelNum, "%i", character.level);  
      for(ch = levelNum; *ch; ch++) {
         glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)*ch);
      }
      glRasterPos3f(34, 65, 0.0);
      sprintf(levelNum, "Level");  
      for(ch = levelNum; *ch; ch++) {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)*ch);
      }
      glColor3f(0.392, 0.765, .937);
      glRasterPos3f(GW-85, 33, 0.0);
      sprintf(levelNum, "Energy");  
      for(ch = levelNum; *ch; ch++) {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)*ch);
      }
      glColor3f(1.0, 0.765, .016);
      glRasterPos3f(GW-85, 65, 0.0);
      sprintf(levelNum, "Experience");  
      for(ch = levelNum; *ch; ch++) {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)*ch);
      }

      //draw level
      Insight.draw();

      //draw objectives
      if (currentLevel && currentLevel->displayObjectives) {
         for (unsigned int i = 0; i < currentLevel->objectives.size(); i++){
            currentLevel->objectives[i]->draw(0.02,0.2 + (float)i/20);
         }
      }
   }

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void HUD::update(float dt)
{
   menutex.update(dt);
   loadtex.update(dt);
   introtex.update(dt);
   outrotex.update(dt);
   Insight.update(dt);

   if (curlvl != character.level){
      if (curlvl < character.level){
         char str[32];
         sprintf(str,"Jump +%d",character.level - curlvl);
         add(str,(float)(rand()%100)/250 + .3,(float)(rand()%10)/50 + .5,1,1,0);
         sprintf(str,"Speed +%d",character.level - curlvl);
         add(str,(float)(rand()%100)/250 + .3,(float)(rand()%10)/50 + .5,0,1,1);
         sprintf(str,"Max Energy +%d",character.level - curlvl);
         add(str,(float)(rand()%100)/250 + .2,(float)(rand()%10)/50 + .5,.2,.2,1);
         if ((character.level)%5 == 0) {
            sprintf(str,"Building +%d",int((character.level - curlvl)/5 + 1));
            add(str,(float)(rand()%100)/250 + .3,(float)(rand()%10)/50 + .5,1,0,1);
            if (character.level == 10){
               Insight.show("New Character Ability: You can now jump up through blocks.",400);
            } else if (character.level == 5){
               Insight.show("New Build Ability: You can now extend a block you have built further in any direction.",500);
            }
         }
      }
      if (!editmode){
         world.add(new obj2d(character.x,character.y,character.z,&(camera.pos),240, false));
         world.add(new obj2d(character.x,character.y,character.z,&(camera.pos),230, false));
         world.add(new obj2d(character.x,character.y,character.z,&(camera.pos),220, false));
         world.add(new obj2d(character.x,character.y,character.z,&(camera.pos),210, false));
         world.add(new obj2d(character.x,character.y,character.z,&(camera.pos),200, false));
      }
      curlvl = character.level;
   }
   for (unsigned int i = 0; i < lvlups.size(); i++){
      lvlups[i].y-=.002;
      lvlups[i].color[3] -= .005;
      if (lvlups[i].color[3] <= 0.2){
         lvlups.erase(lvlups.begin()+i+1);
      }
   }

   if (currentLevel && &(currentLevel->objectives[0]->triggers)) {
      if(!strcmp(currentLevel->levelName, "KOG")){
         if(currentLevel->objectives[0]->isComplete() & !end_flag){
            end_flag = true;
            for (int i = 0; i < 30; i++) {
               world.add(new ParticleSystem(pnt3d(
                        character.x + rand()%20 - 10, 
                        character.y + rand()%30,
                        character.z + rand()%20 - 10), 
                     500, (char *)"textures/lightsource.jpg"));
            }
         }
      }
      else if(!strcmp(currentLevel->levelName, "tutorial")){
         if(currentLevel->objectives.size() > 0 && currentLevel->objectives[0]->triggers.size() > 0 
             && !currentLevel->displayObjectives && currentLevel->objectives[0]->triggers[0]->hit){
           currentLevel->displayObjectives = true;
         }
         for (unsigned int i = 0; i < currentLevel->objectives.size(); i++){
            if(currentLevel->objectives[i]->isComplete() && !Insight.enabled){
              download.id = 0;
              Insight.enabled = true;
              Insight.showFirst("With your help, we can harness all of KOG's energy and shut KOG down.", 500);
              Insight.showFirst("This cannot happen, KOG must be taken down before it is too late.", 400);
              Insight.showFirst("It was KOGs intention to use you to harvest energy from the world.", 400);
              Insight.showFirst("You are the first prototype for a new energy collection device.", 400);
              Insight.showFirst("Hello Project Lume I am Insight! I am an AI created to stop KOG.", 400);
              Insight.showFirst("Download Complete...", 200);
            }
         }
         for(unsigned int i = 0; i < currentLevel->objectives[0]->triggers.size(); i++){//download bar for tuorial
           if(currentLevel->objectives.size() > 0 && currentLevel->objectives[0]->triggers.size() > 0 
               && !currentLevel->objectives[0]->triggers[i]->downloadbar && currentLevel->objectives[0]->triggers[i]->hit){
             char texname[128];
             if (currentLevel->objectives[0]->currentComplete < 10) {
                sprintf(texname, "textures/d%d0.jpg", currentLevel->objectives[0]->currentComplete + 1);
                download.load(texname);
             }
             
             /*sprintf(texname, "textures/d%d0.jpg", currentLevel->objectives[0]->currentComplete + 1);
             
             pnt3d dir = pnt3d(camera.pos.x/camera.pos.mag(),0,camera.pos.z/camera.pos.mag());
             if(pow(dir.x,2) > pow(dir.z,2)){
               dir.z = 0;
             }
             else{
               dir.x = 0;
             }
             
             world.add(new billboard2d(currentLevel->objectives[0]->triggers[i]->x, currentLevel->objectives[0]->triggers[i]->y + 2, 
                                 currentLevel->objectives[0]->triggers[i]->z, LoadTexture(texname), 2, 1, 650, dir));*/
             currentLevel->objectives[0]->triggers[i]->downloadbar = true;
           }
         }
        }
        else if(!strcmp(currentLevel->levelName, "suburbs")){
          if(currentLevel->objectives.size() > 0 && currentLevel->objectives[0]->isComplete() && !currentLevel->objectives[0]->insightPosted){
            currentLevel->objectives[0]->insightPosted = true;
            Insight.show("Grav lifts active in...", 100);
            Insight.show("3..", 50);
            Insight.show("2..", 50);
            Insight.show("1..", 50);
            Insight.show("The grav lifts are now active!", 400);
            Insight.show("Use the grav lifts to reach KOG's consoles and steal the encryption keys.", 400);
          }
	      }
      }
}

void HUD::drawLoadScreen() {
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0, GW, 0, GH);
   glScalef(1, -1, 1);
   glTranslatef(0, -GH, 0);
   glMatrixMode(GL_MODELVIEW);

   glEnable(GL_TEXTURE_2D);
   glColor3f(1.0, 1.0, 1.0);
   glBindTexture(GL_TEXTURE_2D, loadtex.id);
   glBegin(GL_POLYGON);
   glTexCoord2f(0,0);
   glVertex3f(0, 0, -0.1);
   glTexCoord2f(1,0);
   glVertex3f(GW, 0, -0.1);
   glTexCoord2f(1,1);
   glVertex3f(GW, GH, -0.1);
   glTexCoord2f(0,1);
   glVertex3f(0, GH, -0.1);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void HUD::drawIntro() {
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0, GW, 0, GH);
   glScalef(1, -1, 1);
   glTranslatef(0, -GH, 0);
   glMatrixMode(GL_MODELVIEW);

   
   glEnable(GL_TEXTURE_2D);
   glColor3f(1.0, 1.0, 1.0);
   glBindTexture(GL_TEXTURE_2D, introtex.id);
   glBegin(GL_POLYGON);
   glTexCoord2f(0,0);
   glVertex3f(0, 0, -0.1);
   glTexCoord2f(1,0);
   glVertex3f(GW, 0, -0.1);
   glTexCoord2f(1,1);
   glVertex3f(GW, GH, -0.1);
   glTexCoord2f(0,1);
   glVertex3f(0, GH, -0.1);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void HUD::drawOutro() {
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0, GW, 0, GH);
   glScalef(1, -1, 1);
   glTranslatef(0, -GH, 0);
   glMatrixMode(GL_MODELVIEW);

   
   glEnable(GL_TEXTURE_2D);
   glColor3f(1.0, 1.0, 1.0);
   glBindTexture(GL_TEXTURE_2D, outrotex.id);
   glBegin(GL_POLYGON);
   glTexCoord2f(0,0);
   glVertex3f(0, 0, -0.1);
   glTexCoord2f(1,0);
   glVertex3f(GW, 0, -0.1);
   glTexCoord2f(1,1);
   glVertex3f(GW, GH, -0.1);
   glTexCoord2f(0,1);
   glVertex3f(0, GH, -0.1);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

int HUD::drawMainMenu(int x, int y) {
   int itemSelected = 0;
   
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0, GW, 0, GH);
   glScalef(1, -1, 1);
   glTranslatef(0, -GH, 0);
   glMatrixMode(GL_MODELVIEW);

   glEnable(GL_TEXTURE_2D);
   glNormal3f(0,0,1);
   glColor3f(1.0, 1.0, 1.0);
   glBindTexture(GL_TEXTURE_2D, menutex.id);
   glBegin(GL_POLYGON);
   glTexCoord2f(0,0);
   glVertex3f(0, 0, -0.1);
   glTexCoord2f(1,0);
   glVertex3f(GW, 0, -0.1);
   glTexCoord2f(1,1);
   glVertex3f(GW, GH, -0.1);
   glTexCoord2f(0,1);
   glVertex3f(0, GH, -0.1);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   char *c;
   char menu[20];

   //New game highlighting
   if (x >= GW/2-42 && x <= GW/2+44 && y > GH-124 && y < GH-109) {
      glColor3f(0.388, 0.776, 0.941);
      itemSelected = NEW_GAME;
   }
   else {
      glColor3f(1, 1, 1);
   }
   glRasterPos2f(GW/2-43, GH-110); 
   strcpy(menu, "New Game");
   for(c = menu; *c; c++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*c);
   }
      
   //Load game highlighting
   if (x >= GW/2-44 && x < GW/2+40 && y > GH-94 && y < GH-79) {
      itemSelected = LOAD_GAME;
      glColor3f(0.388, 0.776, 0.941);
      loadGameMenu();
   }
   else {
      glColor3f(1, 1, 1);
   }
   glRasterPos2f(GW/2-45, GH-80);
   strcpy(menu, "Load Game");
   for(c = menu; *c; c++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*c);
   }

   //Exit highlighting
   if (x >= GW/2-14 && x <= GW/2+15 && y > GH-64 && y < GH-49) {
      itemSelected = EXIT;
      glColor3f(0.388, 0.776, 0.941);
   }
   else {
      glColor3f(1, 1, 1);
   }
   glRasterPos2f(GW/2-15, GH-50);
   strcpy(menu, "Exit");
   for(c = menu; *c; c++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*c);
   }

   //These only happen if the game has been paused
   if (bPause) {
      //Resume highlighting
      if (x >= GW/2-31 && x <= GW/2+31 && y > GH-184 && y < GH-169) {
         itemSelected = RESUME;
         glColor3f(0.388, 0.776, 0.941);
      }
      else {
         glColor3f(1, 1, 1);
      }
      glRasterPos2f(GW/2-32, GH-170); 
      strcpy(menu, "Resume");
      for(c = menu; *c; c++) {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*c);
      }

      //Save highlighting
      if (x >= GW/2-20 && x <= GW/2+19 && y > GH-154 && y < GH-139) {
         itemSelected = SAVE_GAME;
         glColor3f(0.388, 0.776, 0.941);
      }
      else {
         glColor3f(1, 1, 1);
      }
      glRasterPos2f(GW/2-21, GH-140); 
      strcpy(menu, "Save");
      for(c = menu; *c; c++) {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*c);
      }
   }

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   return itemSelected;
}

int HUD::loadGameMenu() {
   //struct dirent *entry;
   DIR *dp;
 
   dp = opendir("saves");
    
   //while((entry = readdir(dp)))
      //puts(entry->d_name);
   
   return 1;
}

void HUD::add(const char* n,float nx, float ny)
{
   lvlups.push_back(lvlup(n,nx,ny));
}

void HUD::add(const char* n,float nx, float ny, float r, float g, float b)
{
   lvlups.push_back(lvlup(n,nx,ny,r,g,b));
}

