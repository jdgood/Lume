#if ! defined _PART_H
#define _PART_H 1

#include "obj.h"

class Particle: public obj {
public:
	Particle(); 
	Particle(char *tex); 
   Particle(pnt3d, pnt3d, pnt3d);
   Particle(pnt3d, pnt3d, pnt3d, float, float rgb[3], float, float);

	// Overriden methods
	void draw();
	void update(float dt);

   // Other methods
   void calcAcc();
   void calcVel();
   void calcPos();

	pnt3d _pos;
	pnt3d _vel;
   pnt3d _acc;
   pnt3d _force;
   float _mass;
   float _gravity;
   float _rgb[3];
   float _lifetime;
   float _init_lifetime;
   float _size;
   float _init_size;
   float dt;
   obj2d o;

};

class ParticleSystem: public obj {
public:
	ParticleSystem(pnt3d s, int n) : _source(s), _num_p(n){
		classId = PART_ID;
      createSystem();
	}
   
   ParticleSystem(pnt3d s, int n, char *tex) : _source(s), _num_p(n){
		classId = PART_ID;
      createSystem(tex);
	}

	ParticleSystem() { }

	// Overriden methods
	void draw();
	void update(float dt);

   // Other methods
   void createSystem();
   void createSystem(char *tex);

	pnt3d _source;
   Particle p;
   std::vector<Particle*> particles;
   //Particle **particles;
   int _num_p;

};

#endif
