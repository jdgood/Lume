#include "particle.h"

/**
 * Creates a random number between 0 inclusive and max. Used in 
 * ParticleSystem::createSystem()
 */
float randomizer(int max) {
   return (rand()%(max * 100) - (max * 50)) / (float)(max * 50);
}

/**
 * Creates a randome number between min & max. Used to find size in
 * Particle::Partice();
 */
float randomizer(float min, float max) {
   return rand()%(int)(max - min) + min;
}

/**
 * The default particle constructor.  Used in ParticleSystem::CreateSystem()
 */
Particle::Particle() {
   classId = OBJ_ID;
   _pos = pnt3d(randomizer(3), randomizer(3), randomizer(3));
   _vel = pnt3d(randomizer(1)/25, randomizer(1)/25, randomizer(1)/25);
   _force = pnt3d(-_vel.x, -_vel.y, -_vel.z);
   _mass = 300;
   calcAcc();
   _rgb[0] = _rgb[1] = _rgb[2] = randomizer(0, 15)/100;
   _size = _init_size = randomizer(1, 5);
   _lifetime = _init_lifetime = randomizer(30, 80);
   o = obj2d((char *)"textures/lightsource.jpg");
}

/**
* Used to make a default Particle with specified texture.
*/
Particle::Particle(char *tex) {
  classId = OBJ_ID;
  _pos = pnt3d(randomizer(1), randomizer(3), randomizer(1));
  _vel = pnt3d(randomizer(1)/25, randomizer(1)/25, randomizer(1)/25);
  _force = pnt3d(-_vel.x, -_vel.y, -_vel.z);
  _mass = 300;
  calcAcc();
  _rgb[0] = _rgb[1] = _rgb[2] = randomizer(0, 15)/100;
  _size = _init_size = randomizer(1, 5);
  _lifetime = _init_lifetime = randomizer(30, 80);
  o = obj2d(tex);
}

/**
 * Used to create a default partice type with specified direction and
 * speed.
 */
Particle::Particle(pnt3d p, pnt3d v, pnt3d a) {
   classId = OBJ_ID;
   _pos = pnt3d(p.x, p.y, p.z);
   _vel = pnt3d(v.x, v.y, v.z);
   _acc = pnt3d(a.x, a.y, a.z);
   _mass = 1;
   _lifetime = 20;
}

/**
 * Uses to create a unique particle.
 */
Particle::Particle(pnt3d p, pnt3d v, pnt3d f, float m, float rgb[3], 
                   float g, float lt) {
   classId = OBJ_ID;
   _pos = pnt3d(p.x, p.y, p.z);
   _vel = pnt3d(v.x, v.y, v.z);
   _force = pnt3d(f.x, f.y, f.z);
   _mass = m;
   _gravity = g;
   calcAcc();
   _rgb[0] = rgb[0]; _rgb[1] = rgb[1]; _rgb[2] = rgb[2];
   _lifetime = lt;
}

/**
 * Physics equation to find the current acceleration.
 */
void Particle::calcAcc() {
   if (_mass == 0) {
      _acc = pnt3d(0, 0, 0);
   }
   else {
      _acc.x = _force.x / _mass;
      _acc.y = _force.y / _mass;
      _acc.z = _force.z / _mass;
   }
}

/**
 * Physics equation to find the current velocity.
 */
void Particle::calcVel() {
   _vel.x = _vel.x + _acc.x * dt;
   _vel.y = _vel.y + _acc.y * dt;
   _vel.z = _vel.z + _acc.z * dt;
}

/**
 * Physics equation to find the current position.
 */
void Particle::calcPos() {
   _pos.x = _pos.x + _vel.x * dt + 0.5 * _acc.x * dt * dt;
   _pos.y = _pos.y + _vel.y * dt + 0.5 * _acc.y * dt * dt;
   _pos.z = _pos.z + _vel.z * dt + 0.5 * _acc.z * dt * dt;
}

/**
 * Updates the position of a single particle.
 */
void Particle::update(float dt) {
   //calcAcc(); //Turn on if Force changes during animation
   this->dt = dt;
   calcVel(); 
   calcPos();
   _lifetime--;
   _size -= _init_size/_init_lifetime;
}

/**
 * Draws a single particle
 */
void Particle::draw() {
   glPushMatrix();
      glColor3f(_rgb[0], _rgb[1], _rgb[2]);
      glTranslatef(_pos.x, _pos.y + 1, _pos.z);
      glScalef(_size, _size, _size);
      //glutSolidCube(0.01);
      o.draw();
   glPopMatrix();
}

/**
 * Updates all particles in system.
 */
void ParticleSystem::update(float dt) {
   for (int i = 0; i < _num_p; i++) {
      particles[i]->update(dt);
   }
}

/**
 * Draws all particles in system.
 */
void ParticleSystem::draw() {
   if (_num_p != 0) {
      for (int i = 0; i < _num_p; i++) {
         if (particles[i]->_lifetime > 0) {
            glPushMatrix(); {
               glTranslatef(_source.x, _source.y, _source.z);
               particles[i]->draw();
            } glPopMatrix();
         }
         else {
            particles.erase(particles.begin()+i);
            _num_p--;
         }
      }
   }
}

/**
 * Creates a default system of particles. Called in the constructor.
 */
void ParticleSystem::createSystem() {
   for (int i = 0; i < _num_p; i++) {
      particles.push_back(new Particle());
   }
}

/**
 * Creates a default system of particles. Called in the constructor.
 */
void ParticleSystem::createSystem(char *tex) {
   for (int i = 0; i < _num_p; i++) {
      particles.push_back(new Particle(tex));
   }
}
