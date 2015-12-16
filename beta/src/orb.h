#if ! defined _ORB_H
#define _ORB_H 1

#include "obj.h"
#include "sound.h"
#include "player.h"
#include "sdata.h"
#include "hud.h"
#include "shader.h"

extern SoundFX sound;
extern Player character;
extern SData world;
extern HUD* hud;
extern Shader* bloom;
extern int lightsourcetexture;

class Orb: public obj {
public:
	Orb(pnt3d pos, int length) : obj(pos.x, pos.y, pos.z), _pos(pos), _length(length), _visible(false), _active(true) {
		classId = ORB_ID;
		_time = 0.00;
		_fintime = 50.0;
		_velocity = pnt3d(0.0, 0.0, 0.0);
		radius = 1;
		completed = false;

		if(length > 0) {
			_trail = new Orb(_pos, _length-1);
		}
	}

	Orb() {
	}

   ~Orb() {
      _trail = NULL;	
   }

	// Overriden methods
	void draw();
	void update(float dt);

	pnt3d _pos;
	pnt3d _velocity;
	int _length;
	bool _visible;
	bool _active;
	bool completed;
	int _orbId;
	long _time;
	long _fintime;

	void updateTrail();
	void drawTrail();
	
	Orb *_trail;
};

/*	This will be the source object that the orbs will come
	from. All that it will need to construct is its location
	and the user's position. */

#define SRC_ID 2
class Source: public obj2d {
public:
   Source(float sx, float sy, float sz, float rad, int orbcount) :
		_pos(pnt3d(sx, sy, sz)), _dx(character.x), _dy(character.y), _dz(character.z),
		_dest(pnt3d(_dz, _dy, _dz)), _drained(false), _radius(rad), _orbcount(orbcount) {
	   radius = rad;
      
      //obj2d stuff
      n = pnt3d(0,1,0);
      x = sx;
      y = sy;
      z = sz;
      offset = 0;
      width = rad*2;
      height = rad*2;
      ref = NULL;
      extex = NULL;
      parent = NULL;
      texture.id = lightsourcetexture;
      lifetime = 0;
      alpha = 1;
      color = pnt3d(.8,.8,1);
      resize();

		for(int i = 0; i < _orbcount; i++) {
			_orbList[i] = Orb(_pos, 15);
			_orbList[i]._orbId = i;
		}

		classId = SRC_ID;
	};
	Source(float sx, float sy, float sz, float &cx, float &cy, float &cz, float rad, int orbcount) :
		_pos(pnt3d(sx, sy, sz)), _dx(cx), _dy(cy), _dz(cz),
		_dest(pnt3d(_dz, _dy, _dz)), _drained(false), _radius(rad), _orbcount(orbcount) {

      //obj2d stuff
      n = pnt3d(0,1,0);
      x = sx;
      y = sy;
      z = sz;
      offset = 0;
      width = rad*2;
      height = rad*2;
      ref = NULL;
      extex = NULL;
      parent = NULL;
      texture.id = lightsourcetexture;
      lifetime = 0;
      alpha = 1;
      color = pnt3d(.8,.8,1);
      resize();

		for(int i = 0; i < _orbcount; i++) {
			_orbList[i] = Orb(_pos, 20);
			_orbList[i]._orbId = i; 
		}

		classId = 0;
		_left_to_add = 0;
	};

   void drawOrbs();

	// Overriden methods
	void update(float dt);
	///bool hitTest(float ix,float iy,float iz);

	pnt3d collisionAvoidance(Orb o);

	pnt3d _pos;
	float &_dx, &_dy, &_dz;
	pnt3d _dest;
	bool _drained;
	Orb _orbList[100];
	int _left_to_add;
	float _radius;
	int _orbcount;
	bool getDrained(){return _drained;};
   void setDrained(bool b){_drained = b; for(int i = 0; i < _orbcount; i++) {_orbList[i]._active = false;}};
};

// now a public function
pnt3d tend_to_place(Orb o, pnt3d dest, float force);
pnt3d limit_velocity(Orb o);

#endif
