#include "script.h"
// write on wall to unlock cheat??


script::script(){
   y = GH;
   enabled = false;
   visible = false;
   curMessage = 0;
   //symbol = LoadTexture((char*)"textures/symbol.jpg");
}

void script::update(float dt){
   if (visible){
      if (y > SCRIPT_LOC*GH){
         y-= 10;
      } else {
         if (messages[curMessage].hold < 0){
            curMessage++;
            if (curMessage >= messages.size()){
               curMessage--;
               visible = false;
            }
         } else {
            messages[curMessage].hold-=dt;
         }
      }
   } else {
      if (y < GH){
         y+= 10;
         if (y >= GH){
            messages.clear();
            curMessage = 0;
         }
      }
   }
}

void script::resize(){
   y = GH;
}

void script::clear(){
   messages.clear();
   curMessage = 0;
}

void script::show(){
   if (!visible) sound.play("insight.ogg");
   visible = enabled;
}

void script::showFirst(const char * text){
   messages.push_back(message(text));
   for (unsigned int i = messages.size() - 1; i > 0; i--){
      messages[i] =  messages[i-1];
   }
   messages[0] = message(text);
   show();
}

void script::showFirst(const char * text, float hold){
   messages.push_back(message(text,hold));
   for (unsigned int i = messages.size() - 1; i > 0; i--){
      messages[i] =  messages[i-1];
   }
   messages[0] = message(text,hold);
   show();
}

void script::show(const char * text){
   messages.push_back(message(text));
   show();
}

void script::show(const char * text, float hold){
   messages.push_back(message(text,hold));
   show();
}

void script::draw(){
   if (!visible && y >= GH) return;
   if (curMessage < 0 && curMessage >= messages.size()) return;
   char *ch;
   glColor3f(1.0, 1.0, 1.0);
   if (curMessage > 0){
      glRasterPos3f(GW*.1, y+(GH*0.16), 0.0);
   } else {
      glRasterPos3f(GW*.1, y+(GH*0.1), 0.0);
   }
   if (!(messages[curMessage].prompt)) return;
   for(ch = messages[curMessage].prompt; *ch; ch++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*ch);
   }
   if (curMessage > 0){
      glColor3f(0.5, 0.5, 0.5);
      glRasterPos3f(GW*.1, y+(GH*0.1), 0.0); 
      for(ch = messages[curMessage-1].prompt; *ch; ch++) {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*ch);
      }
   }
}

void script::drawBack(){
   if (!visible && y >= GH) return;
   glColor4f(0.0, 0.0, 0.0, 0.4);
   glBegin(GL_POLYGON);
   glTexCoord2f(1,1);
   glVertex3f(0, y, -0.2);
   glTexCoord2f(0,1);
   glVertex3f(GW, y, -0.2);
   glTexCoord2f(0,0);
   glVertex3f(GW, GH, -0.2);
   glTexCoord2f(1,0);
   glVertex3f(0, GH, -0.2);
   glEnd();   
}

