#include "util.h"
#include "obj.h"
#include "player.h"
#include "shader.h"
#include "importMap.h"

extern Player character;
extern Shader* shader;
extern FBO* fbo;
extern bool gridmode;
extern bool bShadows;
extern bool editmode;
extern int edittex;
extern float editsize;
extern pnt3d editdir;
extern pnt3d editoffset;
extern bool BLUR_ON;
extern bool BLOOM_ON;
extern bool fullScreen;
extern bool fullLoad;
extern float mouseSensitivity;
extern char curStage[128];
extern int currentobjectivenum;
extern char currentobjectivename[128];
extern Level* currentLevel;

typedef struct Color{
   float r;
   float g;
   float b;
} Color;

typedef struct out{
   char text[20];
   Color color;
   out(const char* n){strcpy(text,n); color.r=0; color.g=1; color.b=0;}; 
} out;

typedef struct intvar{
  char name[20];
  int* data;
  intvar(const char* n,int* f):data(f){strcpy(name,n);};
} intvar;

typedef struct floatvar{
  char name[20];
  float* data;
  floatvar(const char* n,float* f):data(f){strcpy(name,n);};
} floatvar;

typedef struct pntvar{
  char name[20];
  pnt3d* data;
  pntvar(const char* n,pnt3d* f):data(f){strcpy(name,n);};
} pntvar;

typedef struct vecvar{
  char name[20];
  vec3d* data;
  vecvar(const char* n,vec3d* f):data(f){strcpy(name,n);};
} vecvar;

typedef struct objvar{
  char name[20];
  obj* data;
  objvar(const char* n,obj* f):data(f){strcpy(name,n);};
} objvar;

void printTest();

class debug {
  public:
     char input[64];
     int len;
     bool visible;
     bool active;//accepting input or not
     debug():input(){len = 0;active = false;visible = false;};
     void draw();
     void add(const char* n,int* i);
     void add(const char* n,float* f);
     void add(const char* n,pnt3d* p);
     void add(const char* n,vec3d* v);
     void add(const char* n,obj* o);
     void addOut(const char* n);
     void addOut(const char* n,int c);//0 norm(green), 1 alert(red), 2 other(blue),
     void type(char key);
     void clearInput();
     void command();
     void search(const char * dir);
     void search(const char * dir, const char * filter);
     void search(const char * dir, const char * filter, const char * defilter);
     void search(const char * dir, const char * filter, const char * defilter, bool direct);
     //void printTest();
  private:
     std::vector<out> output;
     std::vector<intvar> ints;
     std::vector<floatvar> floats;
     std::vector<pntvar> pnts;
     std::vector<vecvar> vecs;
     std::vector<objvar> objs;
};
