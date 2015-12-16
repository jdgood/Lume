#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "SOIL.h"

#if 1
#include <GL/glut.h>
#endif

#if 0
#include <GLUT/glut.h>
#include <OPENGL/gl.h>
#include <OPENGL/glext.h>
#endif

using namespace std;

int GW;
int GH;
int gridW, gridH;

int tnum, tcurrent;//amount of textures uploaded, current texture
char tnames[64][64];//list of texture names
char currentType[64];
int menu, texturemenu, typemenu, rampmenu, billboardmenu, energymenu, modelmenu;
int border;

float theta;
float phi;
float lastx, lasty;
float viewx, viewy, viewz;
float radius;
float movex, movey, movez;
int click;
int gView;//0=xy 1=yz 2=xz
int newBrush, r;
float zoom;
float snapToGrid;
float gridmovex[3], gridmovey[3];

int mainWindow, gridWindow,previewWindow,textureWindow;

void texture_select(int value);
void mymenu(int value);
void initVars();

class Brush{
   public:
      char type[64];
      float x1,y1,z1,x2,y2,z2;
      int textureNum;
      void draw();
};// rectangles with collision

void Brush::draw() {
      /*
      v1 x1, y1, z1
      v2 x1, y1, z2
      v3 x1, y2, z1
      v4 x1, y2, z2
      v5 x2, y2, z2
      v6 x2, y1, z2
      v7 x2, y1, z1
      v8 x2, y2, z1
      */

      glBindTexture(GL_TEXTURE_2D, textureNum);
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(x1, y1, z1);//v1
         glTexCoord2f(0.0, 1.0);
         glVertex3f(x1, y2, z1);//v3
         glTexCoord2f(1.0, 1.0);
         glVertex3f(x2, y2, z1);//v8
         glTexCoord2f(1.0, 0.0);
         glVertex3f(x2, y1, z1);//v7
      glEnd();
      
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(x2, y1, z1);//v7
         glTexCoord2f(0.0, 1.0);
         glVertex3f(x2, y2, z1);//v8
         glTexCoord2f(1.0, 1.0);
         glVertex3f(x2, y2, z2);//v5
         glTexCoord2f(1.0, 0.0);
         glVertex3f(x2, y1, z2);//v6
      glEnd();
      
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(x2, y1, z2);//v6
         glTexCoord2f(0.0, 1.0);
         glVertex3f(x2, y2, z2);//v5
         glTexCoord2f(1.0, 1.0);
         glVertex3f(x1, y2, z2);//v4
         glTexCoord2f(1.0, 0.0);
         glVertex3f(x1, y1, z2);//v2
      glEnd();
      
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(x1, y1, z2);//v2
         glTexCoord2f(0.0, 1.0);
         glVertex3f(x1, y2, z2);//v4
         glTexCoord2f(1.0, 1.0);
         glVertex3f(x1, y2, z1);//v3
         glTexCoord2f(1.0, 0.0);
         glVertex3f(x1, y1, z1);//v1
      glEnd();
      
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(x1, y2, z1);//v3
         glTexCoord2f(0.0, 1.0);
         glVertex3f(x1, y2, z2);//v4
         glTexCoord2f(1.0, 1.0);
         glVertex3f(x2, y2, z2);//v5
         glTexCoord2f(1.0, 0.0);
         glVertex3f(x2, y2, z1);//v8
      glEnd();
      
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(x1, y1, z1);//v1
         glTexCoord2f(0.0, 1.0);
         glVertex3f(x1, y1, z2);//v2
         glTexCoord2f(1.0, 1.0);
         glVertex3f(x2, y1, z2);//v6
         glTexCoord2f(1.0, 0.0);
         glVertex3f(x2, y1, z1);//v7
      glEnd();
}

vector<Brush> geometry;
Brush curr;
Brush prev;
bool undo = true;
int cBrush;

GLint LoadTexture(char* image_file, int texID) {
   int r = SOIL_load_OGL_texture(image_file, 0, 0, SOIL_FLAG_POWER_OF_TWO);
   if(r == 0){
      printf("Error loading: %s\n", image_file);
   }
   return r;
}

void exportMap(char* name){
   FILE* map;
   map = fopen(name, "w");
   char buffer[1024];

   for(int i = 0; i < geometry.size(); i++){
      if(geometry[i].x2 - geometry[i].x1 && geometry[i].y2 - geometry[i].y1 && geometry[i].z2 - geometry[i].z1){
         sprintf(buffer, "%f %f %f %f %f %f %s %s", geometry[i].x1, 
            geometry[i].y1, geometry[i].z1, geometry[i].x2, geometry[i].y2,
            geometry[i].z2, tnames[geometry[i].textureNum], geometry[i].type);
         fputs(buffer, map);
         fputs("\n", map);
      }
   }

   fclose(map);
}

int exists(char* t){
   for(int i = 1; i < tnum; i++){
      if(!strcmp(tnames[i], t)){
         return i;
      }
   }
   return -1;
}

void importMap(char* name){
   Brush tempbrush;
   FILE* map;
   char buffer[1024];
   char* token;
   int tempNum;
   map = fopen(name, "r");   
   char tbuffer[64];
   
   fgets (buffer , 1024 , map);
   
   while (!feof(map)){
      tempbrush = Brush();
      token = strtok(buffer, " ");//x1
      strcpy(tbuffer, token);
      tempbrush.x1 = atof(tbuffer);
      token = strtok(NULL, " ");//y1
      strcpy(tbuffer, token);
      tempbrush.y1 = atof(tbuffer);
      token = strtok(NULL, " ");//z1
      strcpy(tbuffer, token);
      tempbrush.z1 = atof(tbuffer);
      token = strtok(NULL, " ");//x2
      strcpy(tbuffer, token);
      tempbrush.x2 = atof(tbuffer);
      token = strtok(NULL, " ");//y2
      strcpy(tbuffer, token);
      tempbrush.y2 = atof(tbuffer);
      token = strtok(NULL, " ");//z2
      strcpy(tbuffer, token);
      tempbrush.z2 = atof(tbuffer);
      //token = strtok(NULL, " ");//type
      //strcpy(tempbrush.type, token);
      //token = strtok(NULL, "\n");//texture name
      //strcpy(tbuffer, token);
      token = strtok(NULL, " ");//texture name
      strcpy(tbuffer, token);
      token = strtok(NULL, "\n");//type
      strcpy(tempbrush.type, token);
      tempNum = exists(tbuffer);
      if(tempNum == -1){
         strcpy(tnames[tnum], tbuffer);
         glutSetWindow(previewWindow);
         LoadTexture(tnames[tnum], tnum);
         glutSetWindow(textureWindow);
         LoadTexture(tnames[tnum], tnum);
         tempbrush.textureNum = tnum;
         tnum++;
      }
      else{
         tempbrush.textureNum = tempNum;
      }
      geometry.push_back(tempbrush);
      fgets (buffer , 1024 , map);
   }
   
   //destroy old menus
   glutDetachMenu(GLUT_RIGHT_BUTTON);
   glutDestroyMenu(texturemenu); 
   glutDestroyMenu(menu);

   //create new menus
   texturemenu = glutCreateMenu(texture_select);
   glutAddMenuEntry("   Insert new texture...   ", 1);
   for(int i = 1; i < tnum; i++){
      glutAddMenuEntry(tnames[i], i + 1);
   }
   
   menu = glutCreateMenu(mymenu);
   glutAddSubMenu("Textures", texturemenu);
   glutAddSubMenu("Types", typemenu);
   glutAddMenuEntry("Quit", 1);
   glutAddMenuEntry("Export", 2);
   glutAddMenuEntry("Import", 3);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   
   fclose (map);
}

void mymenu(int value){
   if(value == 2){
      char name[64];
      printf("Enter file name: ");
      scanf("%s", name);
      exportMap(name);
   }
   else if(value == 3){
      char* name;
      printf("Enter file name: ");
      scanf("%s", name);
      importMap(name);
   }
   else if(value == 1) exit(0);
}

void reshape2(int w, int h){
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
   glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, 1.0f * w / h, .1, 5000);
	glMatrixMode(GL_MODELVIEW);
}

void reshapeGrid(int w, int h){
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
   glViewport(0, 0, w, h);
   float wh = (float)w/h;
	// Set the clipping volume
	glOrtho((-2*zoom)*wh + gridmovex[gView], 
         (2*zoom)*wh + gridmovex[gView], 
         -2*zoom + gridmovey[gView], 
         2*zoom + gridmovey[gView], -1, 1);
   //gluPerspective(45, 1.0f * w / h, .1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void reshape(int w,int h){
   glMatrixMode(GL_PROJECTION);
	GW = w;
	GH = h;

   glutSetWindow(mainWindow);
	glLoadIdentity();
	gluPerspective(45, 1.0 * GW / GH, 1, 350);
   glMatrixMode(GL_MODELVIEW);
   glViewport(0, 0, w, h);

	glutSetWindow(gridWindow);
	glutPositionWindow(border,border);
	gridW = w-2*border;
	gridH = h/2 - border*3/2;
	glutReshapeWindow(gridW, gridH);
	reshapeGrid(gridW, gridH);
   glutPostRedisplay();

	glutSetWindow(previewWindow);
	glutPositionWindow(border,(h+border)/2);
	glutReshapeWindow(w/2-border*3/2, h/2 - border*3/2);
	reshape2(w/2-border*3/2,h/2 - border*3/2);
   glutPostRedisplay();

	glutSetWindow(textureWindow);
	glutPositionWindow((w+border)/2,(h+border)/2);
	glutReshapeWindow(w/2-border*3/2,h/2 - border*3/2);
	reshape2(w/2-border*3/2,h/2 - border*3/2);
   glutPostRedisplay();
}

void coreWindow(){
	glutSetWindow(mainWindow);   
   glClearColor(0.0,0.0,0.0,1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
}

/*
 * Grid Window
 */ 
float snapRound(float in){//snaps any changes to current grid setting
   return (float)(round(in));
}
 
float g2py(int y){
	return gridH - 1 - y;
}

float g2wx(int x){
   return zoom * 2 * ((float)(2*gridW*x) / (gridH*(gridW-1)) - ((float)gridW / (gridH))) + gridmovex[gView];
}

float g2wy(int y){
	float e = (float(gridH) - 1) / 2;
	float f = (float(gridH) - 1) / 2;
	y = g2py(y);

	return ((y - f) / e)*zoom*2 + gridmovey[gView];//*zoom/2 + gridmovey;
}

float min(float x, float y){
   return x<y?x:y;
}

float max(float x, float y){
   return x>y?x:y;
}

void optimize(Brush* b){
   float minx = min(b->x1, b->x2);
   float miny = min(b->y1, b->y2);
   float minz = min(b->z1, b->z2);
   float maxx = max(b->x1, b->x2);
   float maxy = max(b->y1, b->y2);
   float maxz = max(b->z1, b->z2);
   b->x1 = minx;
   b->y1 = miny;
   b->z1 = minz;
   b->x2 = maxx;
   b->y2 = maxy;
   b->z2 = maxz;
}

int position(float x, float y){//0 for repo 1 for left 2 for right 3 for top 4 for down
   if(gView == 0){//xy
      if(x > max(geometry[cBrush].x1, geometry[cBrush].x2))//point right of brush
         return 2;
      if(x < min(geometry[cBrush].x1, geometry[cBrush].x2))//point left of brush
         return 1;
      if(y > max(geometry[cBrush].y1, geometry[cBrush].y2))//point above brush
         return 3;
      if(y < min(geometry[cBrush].y1, geometry[cBrush].y2))//point below brush
         return 4;
   }
   else if(gView == 1){//yz
      if(x > max(geometry[cBrush].z1, geometry[cBrush].z2))//point right of brush
         return 2;
      if(x < min(geometry[cBrush].z1, geometry[cBrush].z2))//point left of brush
         return 1;
      if(y > max(geometry[cBrush].y1, geometry[cBrush].y2))//point above brush
         return 3;
      if(y < min(geometry[cBrush].y1, geometry[cBrush].y2))//point below brush
         return 4;
   }
   else if(gView == 2){//xz
      if(x > max(geometry[cBrush].x1, geometry[cBrush].z2))//point right of brush
         return 2;
      if(x < min(geometry[cBrush].x1, geometry[cBrush].z2))//point left of brush
         return 1;
      if(y > max(geometry[cBrush].z1, geometry[cBrush].z2))//point above brush
         return 3;
      if(y < min(geometry[cBrush].z1, geometry[cBrush].z2))//point below brush
         return 4;
   }
   return 0;
}

void mouseGrid(int button, int state, int x, int y) {
   glutSetWindow(gridWindow);
   if (button == GLUT_LEFT_BUTTON) {
      if (state == GLUT_DOWN) { /* if the left button is clicked */
         if(cBrush == -1){//create new brush
            Brush temp = *(new Brush());
            temp.textureNum = tcurrent;
            //strcpy(temp.type, currentType);
            strcpy(temp.type, "Cube");
            if(gView == 0){//xy
               temp.x1 = g2wx(x);
               temp.y1 = g2wy(y);
               temp.x2 = g2wx(x);
               temp.y2 = g2wy(y);
               temp.z1 = snapToGrid;
               temp.z2 = -snapToGrid;
            }
            if(gView == 1){//yz
               temp.z1 = g2wx(x);
               temp.y1 = g2wy(y);
               temp.z2 = g2wx(x);
               temp.y2 = g2wy(y);
               temp.x1 = snapToGrid;
               temp.x2 = -snapToGrid;
            }
            else if(gView == 2){//xz
               temp.x1 = g2wx(x);
               temp.z1 = g2wy(y);
               temp.x2 = g2wx(x);
               temp.z2 = g2wy(y);
               temp.y1 = snapToGrid;
               temp.y2 = -snapToGrid;
            }
            geometry.push_back(temp);
            click = 0;
            newBrush = 1;
            cBrush = geometry.size() - 1;
         }
         else{
            lastx = g2wx(x);
            lasty = g2wy(y);
            r = position(lastx,lasty);
         }
      }
      else if(state == GLUT_UP){//mouse release
         prev.x1 = curr.x1;
         prev.y1 = curr.y1;
         prev.z1 = curr.z1;
         prev.x2 = curr.x2;
         prev.y2 = curr.y2;
         prev.z2 = curr.z2;

         curr.x1 = snapRound(geometry[cBrush].x1);
         curr.y1 = snapRound(geometry[cBrush].y1);
         curr.z1 = snapRound(geometry[cBrush].z1);
         curr.x2 = snapRound(geometry[cBrush].x2);
         curr.y2 = snapRound(geometry[cBrush].y2);
         curr.z2 = snapRound(geometry[cBrush].z2);

         geometry[cBrush].x1 = snapRound(geometry[cBrush].x1);
         geometry[cBrush].y1 = snapRound(geometry[cBrush].y1);
         geometry[cBrush].z1 = snapRound(geometry[cBrush].z1);
         geometry[cBrush].x2 = snapRound(geometry[cBrush].x2);
         geometry[cBrush].y2 = snapRound(geometry[cBrush].y2);
         geometry[cBrush].z2 = snapRound(geometry[cBrush].z2);
         optimize(&geometry[cBrush]);
         newBrush = 0;
         glutPostRedisplay();
         glutSetWindow(previewWindow);
      }
   }
   else if (button == 3){//scroll wheel up
      zoom/=1.1;
   }
   else if (button == 4){//scroll wheel down
      zoom*=1.1;
   }
   else if(button == GLUT_RIGHT_BUTTON){//ignore if not left click
      if(state == GLUT_DOWN) click = 1;
      if(state == GLUT_UP) click = 0;
   }
   glutPostRedisplay();
}

void mouseMoveGrid(int x, int y){
	glutSetWindow(gridWindow);
   if(click){
      return;
   }
   if(newBrush == 1){//sizing newly created brush
      if(gView == 0){//xy
         geometry[cBrush].x1 = g2wx(x);
         geometry[cBrush].y1 = g2wy(y);
      }
      else if(gView == 1){//yz
         geometry[cBrush].z2 = g2wx(x);
         geometry[cBrush].y2 = g2wy(y);
      }
      else if(gView == 2){//xz
         geometry[cBrush].x1 = g2wx(x);
         geometry[cBrush].z1 = g2wy(y);
      }
   }
   else{//modifying or moving brush
      float tempx = g2wx(x);
      float tempy = g2wy(y);
      float dx = tempx - lastx;
      float dy = tempy - lasty;
      if(r == 0){//reposition
         if(gView == 0){//xy
            geometry[cBrush].x1 += tempx - lastx;
            geometry[cBrush].y1 += tempy - lasty;
            geometry[cBrush].x2 += tempx - lastx;
            geometry[cBrush].y2 += tempy - lasty;
         }
         else if(gView == 1){//yz
            geometry[cBrush].z1 += tempx - lastx;
            geometry[cBrush].y1 += tempy - lasty;
            geometry[cBrush].z2 += tempx - lastx;
            geometry[cBrush].y2 += tempy - lasty;
         }
         else if(gView == 2){//xz
            geometry[cBrush].x1 += tempx - lastx;
            geometry[cBrush].z1 += tempy - lasty;
            geometry[cBrush].x2 += tempx - lastx;
            geometry[cBrush].z2 += tempy - lasty;
         }
      }
      else if(r == 1){//extend left
         if(gView == 0 || gView == 2){//xy or xz
            geometry[cBrush].x1 += dx;
            if(geometry[cBrush].x1 > geometry[cBrush].x2 - snapToGrid){
               geometry[cBrush].x1 = geometry[cBrush].x2 - snapToGrid;
            }
         }
         else if(gView == 1){//yz
            geometry[cBrush].z1 += dx;
            if(geometry[cBrush].z1 > geometry[cBrush].z2 - snapToGrid){
               geometry[cBrush].z1 = geometry[cBrush].z2 - snapToGrid;
            }
         }
      }
      else if(r == 2){//extend right
         if(gView == 0 || gView == 2){//xy or xz
            geometry[cBrush].x2 += dx;
            if(geometry[cBrush].x2 < geometry[cBrush].x1 + snapToGrid){
               geometry[cBrush].x2 = geometry[cBrush].x1 + snapToGrid;
            }
         }
         else if(gView == 1){//yz
            geometry[cBrush].z2 += dx;
            if(geometry[cBrush].z2 < geometry[cBrush].z1 + snapToGrid){
               geometry[cBrush].z2 = geometry[cBrush].z1 + snapToGrid;
            }
         }
      }
      else if(r == 3){//extend up
         if(gView == 0 || gView == 1){//xy or yz
            geometry[cBrush].y2 += dy;
            if(geometry[cBrush].y2 < geometry[cBrush].y1 + snapToGrid){
               geometry[cBrush].y2 = geometry[cBrush].y1 + snapToGrid;
            }
         }
         else if(gView == 2){//xz
            geometry[cBrush].z2 += dy;
            if(geometry[cBrush].z2 < geometry[cBrush].z1 + snapToGrid){
               geometry[cBrush].z2 = geometry[cBrush].z1 + snapToGrid;
            }
         }
      }
      else if(r == 4){//extend down
         if(gView == 0 || gView == 1){//xy or yz
            geometry[cBrush].y1 += dy;
            if(geometry[cBrush].y1 > geometry[cBrush].y2 - snapToGrid){
               geometry[cBrush].y1 = geometry[cBrush].y2 - snapToGrid;
            }
         }
         else if(gView == 2){//xz
            geometry[cBrush].z1 += dy;
            if(geometry[cBrush].z1 > geometry[cBrush].z2 - snapToGrid){
               geometry[cBrush].z1 = geometry[cBrush].z2 - snapToGrid;
            }
         }
      }
      lastx = g2wx(x);
      lasty = g2wy(y);
   }
   glutPostRedisplay();
   glutSetWindow(previewWindow);
   glutPostRedisplay();
}

void draw2d(Brush* b){
   if(gView == 0){//xy
      glBegin(GL_LINE_LOOP);
         glVertex3f(b->x1, b->y1, 0);//v1
         glVertex3f(b->x1, b->y2, 0);//v3
         glVertex3f(b->x2, b->y2, 0);//v8
         glVertex3f(b->x2, b->y1, 0);//v7
      glEnd();
   }
   else if(gView == 1){//yz
      glBegin(GL_LINE_LOOP);
         glVertex3f(b->z1, b->y1, 0);//v7
         glVertex3f(b->z1, b->y2, 0);//v8
         glVertex3f(b->z2, b->y2, 0);//v5
         glVertex3f(b->z2, b->y1, 0);//v6
      glEnd();
   }
   else if(gView == 2){//xz
       glBegin(GL_LINE_LOOP);
         glVertex3f(b->x1, b->z1, 0);//v3
         glVertex3f(b->x1, b->z2, 0);//v4
         glVertex3f(b->x2, b->z2, 0);//v5
         glVertex3f(b->x2, b->z1, 0);//v8
      glEnd();
   }
}

void renderBitmapString(float x, float y, void *font,char *string)
{
   char *c;
   // set position to start drawing fonts
   glRasterPos3f(x, y, 0);
   // loop all the characters in the string
   for (c=string; *c != '\0'; c++) {
      glutBitmapCharacter(font, *c);
      if(*c == '.'){
         c++;
         glutBitmapCharacter(font, *c);
         break;
      }
   }
}

void grid(){
   char lineNum[64];
   reshape(GW,GH);
   
   glutSetWindow(gridWindow);
   glClearColor(1.0,1.0,1.0,1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glPushMatrix();
   int scale = (int)(zoom/10);
   if (scale < 1) scale = 1;
   glScalef(scale,scale,scale);
   glColor3f(.7,.7,.7);
   renderBitmapString(0,0,GLUT_BITMAP_TIMES_ROMAN_24,(char*)"(0,0)");
	for(float i = -1000; i < 1001; i+=snapToGrid){
	   if(fmodf(i, snapToGrid*10) == 0){
	      glLineWidth(2);
	   }
	   else{
	      glLineWidth(1);
	   }
      if (fmodf(i, snapToGrid*5)== 0){
	   sprintf(lineNum, "%f", i*scale);
	   renderBitmapString(0,i,GLUT_BITMAP_8_BY_13,lineNum);
	   renderBitmapString(i,0,GLUT_BITMAP_8_BY_13,lineNum);
      }
	   glBegin(GL_LINES);
         glVertex3f(i, -1000, 0);
         glVertex3f(i, 1001, 0);
      glEnd();
      glBegin(GL_LINES);
         glVertex3f(-1000, i, 0);
         glVertex3f(1001, i, 0);
      glEnd();
	}
   glPopMatrix();
	
   for(float i = 0; i < geometry.size(); i++){
      if(i == cBrush){
         glLineWidth(4);
         glColor3f(1,0,0);
      }
      else{
         glLineWidth(3);
         glColor3f(0,0,0);
      }
      draw2d(&geometry[i]);
   }
   glutSwapBuffers();
   
}

void snap_to_select(int value){
   glutSetWindow(gridWindow);
   snapToGrid = (float)value*.1;
   glutPostRedisplay();
}

/*
 * Preview Window
 */

float toRadian(float x){
   return (x * 3.141592653589793) / 180.0;
}

void preview(){
	glutSetWindow(previewWindow);
   glClearColor(1.0,1.0,1.0,1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   glLoadIdentity();

   gluLookAt(movex, movey, movez, movex + viewx, viewy + movey, movez + viewz, 0, 1, 0);
   
   //glColor3f(1,1,1);
   //makeSkyBox();
   //drawGround();
   for(int i = 0; i < geometry.size(); i++){
      if(i == cBrush){
         glColor3f(1,.66,.66);
      }
      else{
         glColor3f(1,1,1);
         glBindTexture(GL_TEXTURE_2D, geometry[i].textureNum);
      }
      geometry[i].draw();
   }

   glPopMatrix();
   glutSwapBuffers();
}

void mousePreview(int button, int state, int x, int y) {
   if (button == GLUT_RIGHT_BUTTON) {
      if (state == GLUT_DOWN) { /* if the left button is clicked */
         lastx = x;
         lasty = y;
         click = 0;
      }
   }
   else{
      click = 1;
   }
}


//the mouse move callback
void mouseMovePreview(int x, int y) {
	glutSetWindow(previewWindow);
   if(click){
      return;
   }
   float dx = x - lastx;
   float dy = y - lasty;
   phi-=dy;
   theta+=dx;
   if(phi < -50)
      phi = -50;
   if(phi > 50)
      phi = 50;
   viewx = radius * cos(toRadian(phi)) * cos(toRadian(theta));
   viewy = radius * sin(toRadian(phi));
   viewz = radius * cos(toRadian(phi)) * cos(toRadian(90.0 - theta));
   lastx = x;
   lasty = y;
   glutPostRedisplay();
}

/*
 * Texture Window
 */

void type_select(int value){
   if(value == 1)
      strcpy(currentType, "Cube");
   else if(value == 2)
      strcpy(currentType, "Arch");
   else if(value == 3)
      strcpy(currentType, "Fog");
   else if(value == 4)
      strcpy(currentType, "Spawn");
   else if(value == 5)
      strcpy(currentType, "Sky");
   else if(value == 6){
   	char teleLevel[50];
   	char in[50];
   	strcpy (teleLevel, "Tele ");
      printf("Enter level name: ");
      scanf("%s", in);
      strcat(teleLevel, in);
      strcpy(currentType, teleLevel);
   }
   else if (value == 7) {
      strcpy(currentType, "Glass");
   }
   else if (value == 8) {
      strcpy(currentType, "Ground");
   }
   else if(value == 9){
   	char objective[50];
   	char in[50];
      strcpy (objective, "Trigger ");
      printf("Enter object name: ");
      scanf("%s", in);
      strcat(objective, in);
      strcpy(currentType, objective);
   }
   if(cBrush != -1)
      strcpy(geometry[cBrush].type, currentType);
}

void ramp_select(int value){
   if(value == 1)
      strcpy(currentType, "Ramp");
   else if(value == 2)
      strcpy(currentType, "Ramp180");
   else if(value == 3)
      strcpy(currentType, "Roof");
   else if(value == 4)
      strcpy(currentType, "Roof180");
      
   if(cBrush != -1)
      strcpy(geometry[cBrush].type, currentType);
}

void billboard_select(int value){
   if(value == 1)
      strcpy(currentType, "Billboard");
   else
      strcpy(currentType, "Billboard180");
      
   if(cBrush != -1)
      strcpy(geometry[cBrush].type, currentType);
}

void energy_select(int value){
   if(value == 1)
      strcpy(currentType, "Energy");
   else
      strcpy(currentType, "LEnergy");
      
   if(cBrush != -1)
      strcpy(geometry[cBrush].type, currentType);
}

void model_select(int value){
   if(value == 1)
      strcpy(currentType, "Model models/lamppost/lamppost.obj ");
   if(value == 2)
      strcpy(currentType, "Model models/monitor/monitor.obj ");

   char in[50];
   printf("Enter direction(+/-x or +/-z): ");
   scanf("%s", in);
   strcat(currentType, in);
   
   if(cBrush != -1)
      strcpy(geometry[cBrush].type, currentType);
}

void texture_select(int value){
   glutSetWindow(textureWindow);
   if(value == 1){
      char texturename[64];
      printf("Enter texture name: ");
      scanf("%s", texturename);
      sprintf(tnames[tnum], "textures/%s.jpg", texturename);
      int r = LoadTexture(tnames[tnum], tnum);
      glutSetWindow(previewWindow);
      LoadTexture(tnames[tnum], tnum);
      glutSetWindow(textureWindow);
      if(r != 0){
         tcurrent = tnum;
         tnum++;
      }

      //destroy old menus
      glutDetachMenu(GLUT_RIGHT_BUTTON);
      glutDestroyMenu(texturemenu); 
      glutDestroyMenu(menu);

      //create new menus
      texturemenu = glutCreateMenu(texture_select);
      glutAddMenuEntry("   Insert new texture...   ", 1);
      for(int i = 1; i < tnum; i++){
         glutAddMenuEntry(tnames[i], i + 1);
      }
	   menu = glutCreateMenu(mymenu);
      glutAddSubMenu("Textures", texturemenu);
      glutAddSubMenu("Types", typemenu);
	   glutAddMenuEntry("Quit", 1);
	   glutAddMenuEntry("Export", 2);
	   glutAddMenuEntry("Import", 3);
	   glutAttachMenu(GLUT_RIGHT_BUTTON);
   }
   else{
      tcurrent = value-1;
   }
   if(cBrush != -1){
      geometry[cBrush].textureNum = tcurrent;
   }
   //display current texture in the texture preview subwindow
   glutPostRedisplay();
   glutSetWindow(previewWindow);
   glutPostRedisplay();
}

void texturePreview(){
	glutSetWindow(textureWindow);

   glClearColor(0.0,0.0,0.0,1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glColor3f(1,1,1);
   renderBitmapString(5,5,GLUT_BITMAP_TIMES_ROMAN_24,currentType);

	glLoadIdentity();
	
   gluLookAt(0, 0, 0, 0, 0, .1, 0, 1, 0);

   /*glColor3f(0.0, 0.0, 0.0);
   glBegin(GL_POLYGON);
      glVertex3f(20, 20, 5);
      glVertex3f(20, -20, 5);
      glVertex3f(-20, -20, 5);
      glVertex3f(-20, 20, 5);
   glEnd();*/

   glColor3f(1.0, 1.0, 1.0);

   if(tcurrent == 0){//no loaded texture
      glBegin(GL_POLYGON);
         glVertex3f(.5, .5, 2);
         glVertex3f(.5, -.5, 2);
         glVertex3f(-.5, -.5, 2);
         glVertex3f(-.5, .5, 2);
      glEnd();
   }
   else if(cBrush==-1){//no loaded texture
      glBindTexture(GL_TEXTURE_2D, tcurrent);
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(-.5, -.5, 2);
         glTexCoord2f(0.0, 1.0);
         glVertex3f(-.5, .5, 2);
         glTexCoord2f(1.0, 1.0);
         glVertex3f(.5, .5, 2);
         glTexCoord2f(1.0, 0.0);
         glVertex3f(.5, -.5, 2);
      glEnd();
   }
   else{
      glBindTexture(GL_TEXTURE_2D, geometry[cBrush].textureNum);
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0);
         glVertex3f(-.5, -.5, 2);
         glTexCoord2f(0.0, 1.0);
         glVertex3f(-.5, .5, 2);
         glTexCoord2f(1.0, 1.0);
         glVertex3f(.5, .5, 2);
         glTexCoord2f(1.0, 0.0);
         glVertex3f(.5, -.5, 2);
      glEnd();
   }
   glutSwapBuffers();
}

/*
 * General Keyboard Function
 */

void keyboard(unsigned char key, int x, int y ){
   switch( key ) {
      case 's':
         movex -= cos(toRadian(theta)) * 5;
         movez -= sin(toRadian(theta)) * 5;
         movey -= sin(toRadian(phi)) * 5; 
         break;
      case 'w':
         movex += cos(toRadian(theta)) * 5;
         movez += sin(toRadian(theta)) * 5;
         movey += sin(toRadian(phi)) * 5;
         break;
      case 'd':
         movex -= sin(toRadian(theta)) * 0.75;
         movez += cos(toRadian(theta)) * 0.75;
         break;
      case 'a':
         movex += sin(toRadian(theta)) * 0.75;
         movez -= cos(toRadian(theta)) * 0.75;
         break;
      case 'f':
         movey += 1;
         break;
      case 'c':
         movey -= 1;
         break;
      case 'u':
         if (undo) {
            geometry[cBrush].x1 = prev.x1;
            geometry[cBrush].y1 = prev.y1;
            geometry[cBrush].z1 = prev.z1;
            geometry[cBrush].x2 = prev.x2;
            geometry[cBrush].y2 = prev.y2;
            geometry[cBrush].z2 = prev.z2;
         }
         else {
            geometry[cBrush].x1 = curr.x1;
            geometry[cBrush].y1 = curr.y1;
            geometry[cBrush].z1 = curr.z1;
            geometry[cBrush].x2 = curr.x2;
            geometry[cBrush].y2 = curr.y2;
            geometry[cBrush].z2 = curr.z2;
         }
         undo = !undo;
         break;
      case 'r'://cycle current brush forward
         cBrush++;
         if(cBrush == geometry.size()){
              cBrush = 0;
         }
         if(geometry.size() == 0){
            cBrush = -1;
         }
         else{
            strcpy(currentType, geometry[cBrush].type);
            tcurrent = geometry[cBrush].textureNum;
         }
         break;
      case 'e'://cycle current brush backward
         cBrush--;
         if(cBrush == -2 || cBrush == -1){
            cBrush = geometry.size() - 1;
         }
         if(geometry.size() == 0){
            cBrush = -1;
         }
         else{
            strcpy(currentType, geometry[cBrush].type);
            tcurrent = geometry[cBrush].textureNum;
         }
         break;
      case 27://esc
         cBrush = -1;
         break;
      case 'p':
         //printf("%f %f %f\n", movex, movey, movez);
         if(gView == 0){
            printf("Current View: xy\n");
         }
         else if (gView == 1){
            printf("Current View: yz\n");
         }
         else{
            printf("Current View: xz\n");
         }
         printf("currentType %s\n", currentType);
         printf("cBrush: %d\n", cBrush);
         /*if(cBrush!=-1){
            printf("brush: %f %f %f %f %f %f %d\n",geometry[cBrush].x1, geometry[cBrush].y1, geometry[cBrush].z1, 
             geometry[cBrush].x2, geometry[cBrush].y2, geometry[cBrush].z2, geometry[cBrush].textureNum);
         }*/
         /*for(int i = 1; i < tnum; i++){
            printf("%s\n",tnames[i]);
         }*/
         break;
      case 'v':
         gView++;
         if(gView == 3){
              gView = 0;
         }
         break;
		  case 'j':
		    gridmovex[gView] -= zoom;
		    break;
		  case 'l':
			  gridmovex[gView] += zoom;
		    break;
		  case 'i':
		    gridmovey[gView] += zoom;
			  break;
		  case 'k':
			  gridmovey[gView] -= zoom;
			  break;
		  case 'm':
	      char inName[64];
         printf("Enter file name: ");
         scanf("%s", inName);
         sprintf(inName, "%s/map", inName);
         initVars();
         geometry.clear();
         importMap(inName);
         break;
      case 'x':
         char outName[64];
         printf("Enter file name: ");
         scanf("%s", outName);
         sprintf(outName, "%s/map", outName);
         exportMap(outName);
         break;
      case '\b':
         if(cBrush!=-1){
            geometry.erase(geometry.begin()+cBrush);
            cBrush = -1;
         }
         break;
      case ' ':
         if(cBrush!=-1){
            Brush temp = *(new Brush());
            temp.textureNum = geometry[cBrush].textureNum;
            strcpy(temp.type, geometry[cBrush].type);
            if(gView == 0){//xy
               temp.x1 = geometry[cBrush].x1 - 1;
               temp.y1 = geometry[cBrush].y1 - 1;
               temp.x2 = geometry[cBrush].x2 - 1;
               temp.y2 = geometry[cBrush].y2 - 1;
               temp.z1 = geometry[cBrush].z1;
               temp.z2 = geometry[cBrush].z2;
            }
            if(gView == 1){//yz
               temp.z1 = geometry[cBrush].z1 - 1;
               temp.y1 = geometry[cBrush].y1 - 1;
               temp.z2 = geometry[cBrush].z2 - 1;
               temp.y2 = geometry[cBrush].y2 - 1;
               temp.x1 = geometry[cBrush].x1;
               temp.x2 = geometry[cBrush].x2;
            }
            else if(gView == 2){//xz
               temp.x1 = geometry[cBrush].x1 - 1;
               temp.z1 = geometry[cBrush].z1 - 1;
               temp.x2 = geometry[cBrush].x2 - 1;
               temp.z2 = geometry[cBrush].z2 - 1;
               temp.y1 = geometry[cBrush].y1;
               temp.y2 = geometry[cBrush].y2;
            }
            geometry.push_back(temp);
            cBrush = geometry.size() - 1;
         }
         break;
      case 'q':
         exit(0);
         break;
    }
    glutSetWindow(previewWindow);
    glutPostRedisplay();
    glutSetWindow(gridWindow);
    glutPostRedisplay();
    glutSetWindow(textureWindow);
    glutPostRedisplay();
}

void initScene(){
	glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
}

void initVars() {
   click = 0;
   cBrush = -1;
   gView = 0;
   newBrush = 0;
   zoom = 10;
   snapToGrid = 1;
   for (int i = 0; i < 3; i++) {
      gridmovex[i] = gridmovey[i] = 0;
   }
   r = 0;

   phi = 0; 
   theta = -90;
   lastx = lasty = 0;
   viewx = viewy = 0;
   viewz = -.1;
   radius = 15;
   movex = movez = 0;
   movey = 1;

   tnum = 1;
   tcurrent = 0;
   strcpy(currentType, "Cube");
   for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 64; j++) {
         tnames[i][j] = '\0';
      }
   }
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	mainWindow = glutCreateWindow("Mapper Alpha");
   glClearColor(1.0, 1.0, 1.0, 1.0);
	glutReshapeFunc(reshape);
	glutDisplayFunc(coreWindow);
   glutKeyboardFunc(keyboard);
   GW = GH = 500;
   border = 6;
   initScene();
   
   gridW = GW-2*border;
	gridH = GH/2 - border*3/2;
   
	gridWindow = glutCreateSubWindow(mainWindow, border, border, gridW, gridH);
   glClearColor(1.0, 1.0, 1.0, 1.0);
   glutMouseFunc(mouseGrid);
   glutMotionFunc(mouseMoveGrid);
   glutKeyboardFunc(keyboard);
	glutDisplayFunc(grid);
	initScene();

   initVars();

   /*
   //int snap_menu = glutCreateMenu(snap_to_select);
   //glutAddMenuEntry(".1", 1);
   //glutAddMenuEntry("1", 10);
   //glutAddMenuEntry("10", 100);
	//menu = glutCreateMenu(mymenu);
	//glutAddSubMenu("Snap To Grid", snap_menu);
	//glutAddMenuEntry("Quit", 1);
	//glutAddMenuEntry("Export", 2);
   //glutAddMenuEntry("Import", 3);
	//glutAttachMenu(GLUT_RIGHT_BUTTON);
	*/

	previewWindow = glutCreateSubWindow(mainWindow, border, (GH+border)/2, GW/2-border*3/2, GH/2 - border*3/2);
   glClearColor(1.0, 1.0, 1.0, 1.0);
   glutMouseFunc(mousePreview);
   glutMotionFunc(mouseMovePreview);
   glutKeyboardFunc(keyboard);
	glutDisplayFunc(preview);
	initScene();
   
	textureWindow = glutCreateSubWindow(mainWindow, (GW+border)/2, (GH+border)/2, GW/2-border*3/2, GH/2 - border*3/2);
   glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(texturePreview);
	glutKeyboardFunc(keyboard);
   
   //texture menu creations
   texturemenu = glutCreateMenu(texture_select);
   glutAddMenuEntry("   Insert new texture...   ", 1);
   rampmenu = glutCreateMenu(ramp_select);
   glutAddMenuEntry("Ramp", 1);
   glutAddMenuEntry("Ramp180", 2);
   glutAddMenuEntry("Roof", 3);
   glutAddMenuEntry("Roof180", 4);
   billboardmenu = glutCreateMenu(billboard_select);
   glutAddMenuEntry("Billboard", 1);
   glutAddMenuEntry("Billboard180", 2);
   energymenu = glutCreateMenu(energy_select);
   glutAddMenuEntry("Energy Source", 1);
   glutAddMenuEntry("Large Energy Source", 2);
   modelmenu = glutCreateMenu(model_select);
   glutAddMenuEntry("Lamppost", 1);
   glutAddMenuEntry("Monitor", 2);
   typemenu = glutCreateMenu(type_select);
   glutAddMenuEntry("Cube", 1);
   glutAddMenuEntry("Arch", 2);
   glutAddMenuEntry("Fog", 3);
   glutAddMenuEntry("Spawn", 4);
   glutAddMenuEntry("Sky", 5);
   glutAddMenuEntry("Glass", 7);
   glutAddMenuEntry("Ground", 8);
   glutAddMenuEntry("Teleport...", 6);
   glutAddMenuEntry("Objective...", 9);
   glutAddSubMenu("Ramp", rampmenu);
   glutAddSubMenu("Billboard", billboardmenu);
   glutAddSubMenu("Energy", energymenu);
   glutAddSubMenu("Models", modelmenu);

	menu = glutCreateMenu(mymenu);
   glutAddSubMenu("Textures", texturemenu);
   glutAddSubMenu("Types", typemenu);
	glutAddMenuEntry("Quit", 1);
   glutAddMenuEntry("Export", 2);
   glutAddMenuEntry("Import", 3);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

	initScene();

	glutMainLoop();
}
