#if ! defined _OBJ_H
#define _OBJ_H 1

#include "util.h"
#include "tex.h"
#include "sprite.h"
#include "objimport.h"
//#include "importMap.h"

extern int mapModTime;
extern int levelLoader;
extern float glowBrightness;

typedef struct int3d {
  int x;
  int y;
  int z;
  int3d():x(0),y(0),z(0){};
  int3d(int in_x,int in_y,int in_z):x(in_x),y(in_y),z(in_z){};
  float mag(){return sqrt(x*x + y*y + z*z);};
} int3d;

typedef struct pnt3d {
  float x;
  float y;
  float z;
  pnt3d():x(0),y(0),z(0){};
  pnt3d(float in_x,float in_y,float in_z):x(in_x),y(in_y),z(in_z){};
  float mag(){return sqrt(x*x + y*y + z*z);};
  //pnt3d operator* (float f){ x = x*f; y = y*f; z = z*f;}
  pnt3d operator* (float f){ return pnt3d(x*f, y*f, z*f); }
  pnt3d operator+ (pnt3d o){ return pnt3d(x+o.x, y+o.y, z+o.z); }
  pnt3d operator- (pnt3d o){ return pnt3d(x-o.x, y-o.y, z-o.z); }
  pnt3d operator/ (float b){ return pnt3d(x/b, y/b, z/b); }
  bool operator== (pnt3d p){ return (p.x == x && p.y == y && p.z == z); }
  void print() { printf("%f %f %f\n",x,y,z); } 
  void norm(){float m = mag(); x/=m; y/=m; z/=m;};
  pnt3d normalize(pnt3d o);
  float dotproduct(pnt3d o);
  pnt3d crossproduct(pnt3d o);
  float distBetween(pnt3d o);
} pnt3d;

extern pnt3d sf;

typedef struct surf {
  std::vector<pnt3d> verts;
  std::vector<int> overts;
  float n[3];
  //surf():verts(),overts(){};
} surf;

typedef struct vec3d {
  float x;
  float y;
  float z;
  float px;
  float py;
  float pz;
  vec3d():x(0),y(0),z(0),px(0),py(0),pz(0){};
  vec3d(float in_x,float in_y,float in_z):x(in_x),y(in_y),z(in_z),px(0),py(0),pz(0){};
  vec3d(float in_x,float in_y,float in_z, float in_px,float in_py,float in_pz):x(in_x),y(in_y),z(in_z),px(in_px),py(in_py),pz(in_pz){};
  float mag(){return sqrt(x*x + y*y + z*z);}
} vec3d;

class drawable {
   public:
     int index;//individual id
     int classId;
     virtual ~drawable() {}
     virtual void draw() = 0;
     virtual void update(float dt) = 0;
     virtual int getTexId(){return -1;}
	  virtual float getX(){return -1;}
	  virtual float getY(){return -1;}
	  virtual float getZ(){return -1;}
	  virtual int getParentI(){return -1;}
	  virtual float getSize(){return -1;}
};

extern pnt3d edgeColor;
extern float waveEffect[8];

class obj : public drawable {
  public:
     std::vector<int3d> buckets;//bucket index for spacial data tructure 
     float x;
     float y;
     float z;
     float radius;
     int cFrame;
     int colored;
     int litColor;
     float wireFrameBrightness;
     bool visible;//still shows grid
     bool hidden;
     std::vector<surf> surface;//surfaces
     std::vector<pnt3d> colors;
     Sprite texture;
     obj():x(0),y(0),z(0),cFrame(0),surface(),colors(){classId=OBJ_ID;colored = 0;litColor = -1;visible = true;hidden = false;wireFrameBrightness = 0.1;};
     obj(float in_x,float in_y,float in_z):x(in_x),y(in_y),z(in_z),cFrame(0),surface(),colors(){
         classId=OBJ_ID;colored = 0;litColor = -1;visible = true;hidden = false;wireFrameBrightness = 0.1;};
     virtual ~obj(){}
     virtual char* getLevelName(){return (char*)"";}
     virtual void draw();
     virtual bool hitTest(float ix,float iy,float iz);
     virtual bool hitCamera(float ix,float iy,float iz);
     virtual bool hitCollision(obj object);
     virtual pnt3d getNorm(float ix,float iy,float iz);
     virtual void update(float dt);
     virtual void click(float cx, float cy, float cz);
     virtual bool doTeleport(){return false;}
     virtual void setLoading(){}
     virtual void setActive(){}
     int getOVert(float cx, float cy, float cz);
     void setColor(int s, int n);
     void fadeColor();
     void fullColor();
     virtual pnt3d getDir(){return pnt3d();};
     //int texture;
     //char* tname;
     float fx;//floor (what you stand on)
     float fy;
     float fz;
     float xmax;//bounds
     float ymax;
     float zmax;
     float xmin;//bounds
     float ymin;
     float zmin;
};

class objmodel : public obj {
  public:
      objmodel(float x,float y,float z):obj(x,y,z){}
  		objmodel(float x,float y,float z,char*objname):obj(x,y,z){classId=MODEL_ID; om = LoadObjMesh(objname);}
  		virtual bool hitTest(float ix,float iy,float iz){return false;}
      virtual bool hitCollision(obj object){return false;}
  		virtual ~objmodel(){}
  		virtual bool doTeleport(){return false;}
  		virtual void setLoading(){}
  		virtual void draw();
  		ObjMesh om;
  		int drawrotation;
  		int flipy;
};

class tele : public objmodel {
  public:
  		tele(float x,float y,float z,char*in_mapName, bool _active):objmodel(x,y,z){strcpy(mapName,in_mapName);classId=TELEPORT_ID; om = LoadObjMesh("models/arch/tele.obj"); active = _active; doTele = false; loading = false;}
  		virtual ~tele(){}
  		bool hitTest(float ix,float iy,float iz){return false;}
      bool hitCollision(obj object);
  		bool doTeleport(){return doTele;}
  		void setLoading(){doTele = false; loading = true;}
  		bool active;
  		bool doTele;
  		bool loading;
  		char mapName[64];
  		void draw();
  		char* getLevelName(){return mapName;}
};

//static float offset2d = 0.001;

class obj2d : public drawable {
	public:
      float x;
      float y;
      float z;
      float width;
      float height;
      float offset;//used to negate overlapping
      float lifetime; //have it disappear after x seconds? else 0
      float alpha;
      bool clip;
      bool fade;
      float fadeOutDist;
      // texture mapping
      float amax;
      float amin;
      float bmax;
      float bmin;
      float radius;
      float flip;
      int objectivenum;
      char objectivename[128];
      vec3d* ref;
      pnt3d n;
      pnt3d color;
      Sprite texture;
		Sprite* extex;
      obj* parent;
      obj2d();
      obj2d(char *tex);
      obj2d(float nx, float ny, float nz,vec3d* r);
      obj2d(float nx, float ny, float nz,vec3d* r,int time, bool insight);
      obj2d(float nx, float ny, float nz, pnt3d dir, vec3d* r,int texid,float size);
		obj2d(float nx, float ny, float nz, obj* o, vec3d* r);
      obj2d(float nx, float ny, float nz, obj* o, vec3d* r, int texid, float size);
      obj2d(float nx, float ny, float nz, obj* o, vec3d* r, Sprite* tex);
      obj2d(float mx, float my, float mz, float nx, float ny, float nz, float w, float h, vec3d* r,char* tex);
      obj2d(float nx, float ny, float nz, obj* o,vec3d* r,int texid,float size, int i);
      obj2d(float nx, float ny, float nz, obj* o,vec3d* r, float size);
      obj2d(float nx, float ny, float nz, obj* o,vec3d* r, char* texname, int frames, float size);
      void resize();
      void setTexCoord(int i);
      float distance();      
      void rotateToNormal();
      virtual void update(float dt);
		void draw();
      void kill();
		int getTexId(){return texture.id;}
		float getX(){return x;}
		float getY(){return y;}
		float getZ(){return z;}
      pnt3d getNorm(){return n;}
		int getParentI(){return parent->index;}
		float getSize(){return width;}
		virtual bool getDrained(){return false;};
      virtual void setDrained(bool b){};
};

obj2d * newTrail(float nx, float ny, float nz,vec3d* r);
objmodel* newModel(float x,float y,float z,float dx,float dy,float dz,char*tname,char*in_objame, int rotation);
tele* newTele(float x,float y,float z,float dx,float dy,float dz,char*tname,char*in_mapName,bool active);
tele* newTele(char*in_mapName, float x, float y, float z);
obj* newCube(float x,float y,float z,float dx,float dy,float dz);
obj* newCube(float x,float y,float z,float dx,float dy,float dz,char* tname);
obj* newRamp(float x,float y,float z,float dx,float dy,float dz,int rotate, int oneeighty, char* texture);
void addCube(obj* o,float dx,float dy,float dz);
#endif
