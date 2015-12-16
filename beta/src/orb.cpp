#include "orb.h"

/*void Source::draw() {
	glPushMatrix();
      glLoadName(-2);
      glEnable (GL_BLEND);
	   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	   glTranslatef(_pos.x, _pos.y, _pos.z);
      
      glRotatef(acos(-n.z)*RAD2DEG, n.y, -n.x, 0);
      glBindTexture(GL_TEXTURE_2D, LoadTexture((char*)"textures/lightsource.jpg"));
      glBegin(GL_POLYGON);
      glColor4f(.8,.8,1,(_drained) ? .4 : 1);
      glTexCoord2f(1,0);
      glVertex3f(_radius,-_radius,0);
      glTexCoord2f(0,0);
      glVertex3f(-_radius,-_radius,0);
      glTexCoord2f(0,1);
      glVertex3f(-_radius,_radius,0);
      glTexCoord2f(1,1);
      glVertex3f(_radius,_radius,0);
      glEnd();

	//glBindTexture(GL_TEXTURE_2D, LoadTexture((char*)"textures/stub.bmp"));
	//gluSphere(gluNewQuadric(), _radius, 32, 32);
      glDisable (GL_BLEND);
	glPopMatrix();

   //drawOrbs();
}*/

int dist_cmp(const void * a, const void * b)
{
	Orb c = *((Orb*)a);
	Orb d = *((Orb*)b);
	pnt3d campos = pnt3d(camera.pos.x, camera.pos.y, camera.pos.z);

	if (c._pos.distBetween(campos) > d._pos.distBetween(campos)) return -1;
	if (c._pos.distBetween(campos) < d._pos.distBetween(campos)) return 1;
	return 0;
}

void Source::drawOrbs(){
	
	//qsort (_orbList, _orbcount, sizeof(Orb), dist_cmp);
	for(int i = 0; i < _orbcount; i++) {
		if(_orbList[i]._active && _orbList[i]._visible) {
			glPushMatrix();
			// orb rotation
			pnt3d def = pnt3d(0.0, 0.0, -1.0);
			pnt3d path = pnt3d(camera.pos.x, camera.pos.y, camera.pos.z) -
								pnt3d(0,0,1);
			path = path.normalize(path);
			pnt3d cross = def.crossproduct(path);
			float radians = def.dotproduct(path);
			radians = acos(radians/(def.mag()*path.mag()));
			radians = radians*(180/M_PI);

			glTranslatef(_orbList[i]._pos.x, _orbList[i]._pos.y, _orbList[i]._pos.z);
			glRotatef(radians, cross.x, cross.y, cross.z);
		
			glEnable (GL_BLEND);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glBindTexture(GL_TEXTURE_2D, lightsourcetexture);
		
			if(!_orbList[i].completed) {
				glBegin(GL_POLYGON);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(-0.30, 0.30, 0.0); //v5
				glTexCoord2f(0.0, 1.0);
				glVertex3f(0.30, 0.30, 0.0); //v6
				glTexCoord2f(1.0, 1.0);
				glVertex3f(0.30, -0.30, 0.0); //v7
				glTexCoord2f(1.0, 0.0);
				glVertex3f(-0.30, -0.30, 0.0); //v8
				glEnd();
			}

			glDisable (GL_BLEND);
			glPopMatrix();

			_orbList[i].drawTrail();
		}
	}
}

void Source::update(float dt) {
	pnt3d v1, v2;

	// Update destination pos to current player pos
	_dest = pnt3d(_dx, _dy, _dz);
   //update camera
   n = pnt3d(camera.pos.x/camera.pos.mag(),camera.pos.y/camera.pos.mag(),camera.pos.z/camera.pos.mag());

	if(_pos.distBetween(_dest) < (_radius + 4)) {
		if(radius > 5){
                   if (!_drained){
                      _drained = true;
                   }
                   character.setCheckpoint(x, y-9, z);
		}
		else{
                   if (!_drained){
                      _drained = true;
                      //hud->Insight.show("Insight: ENERGYYYYY!");
                      character.addExp(10);
                      alpha = .2;
                   }
                   character.setCheckpoint(x, y, z);
                }
	}

	if(_drained) {
		//for each(Orb i in _orbList) {
		for(int i = 0; i < _orbcount; i++) {
			if(_orbList[i]._active) {	
				_orbList[i]._visible = true;

				v1 = collisionAvoidance(_orbList[i]);
				v2 = tend_to_place(_orbList[i], _dest, 100.0);
				_orbList[i]._velocity = _orbList[i]._velocity + v1 + v2;
				_orbList[i]._velocity = limit_velocity(_orbList[i]);
				_orbList[i]._pos = _orbList[i]._pos + _orbList[i]._velocity;

					if(_orbList[i]._pos.distBetween(_dest) < 2.0) {
						_orbList[i]._time += dt;

						//if(false) {						
						if( _orbList[i]._time > 20 ) {
							if(!_orbList[i].completed) {
								_orbList[i].completed = true;
                        switch(rand()%4){							
								case 0:sound.play("pingA.ogg",_dest.x,_dest.y,_dest.z);break;
                        case 1:sound.play("pingD.ogg",_dest.x,_dest.y,_dest.z);break;
                        case 2:sound.play("pingF.ogg",_dest.x,_dest.y,_dest.z);break;
                        case 3:sound.play("pingC.ogg",_dest.x,_dest.y,_dest.z);break;                      
                        }							
								character.addExp(1);
								character.addEnergy(1);
                        currentLevel->energyCollected++;
							}
							_orbList[i]._pos.x = _dest.x;
							_orbList[i]._pos.y = _dest.y;
							_orbList[i]._pos.z = _dest.z;

							Orb* tail = _orbList[i]._trail;

							while(tail->_length > 0) {
								if(_dest.distBetween(tail->_pos) < 0.25) tail->completed = true;
								tail = tail->_trail; 
							}

							if(_orbList[i]._pos.distBetween(tail->_pos) < 0.5) {				
								_orbList[i]._active = false;
								_orbList[i]._visible = false;
							}
						}
					}
			}

			_orbList[i].x = _orbList[i]._pos.x;
			_orbList[i].y = _orbList[i]._pos.y;
			_orbList[i].z = _orbList[i]._pos.z;

			if(_orbList[i]._visible) {
				_orbList[i].updateTrail();
			}
		}
	}
}

void Orb::draw() {

}

void Orb::update(float index) {

}

void Orb::updateTrail() {
	if(_pos.distBetween(_trail->_pos) > 0) {
		_trail->_visible = true;
		_trail->_velocity = _trail->_velocity + tend_to_place(*_trail, _pos, 10.0);
		_trail->_velocity = limit_velocity(*_trail);
		_trail->_pos = _trail->_pos + _trail->_velocity;

		if(_trail->_length > 0) {
			_trail->updateTrail();
		}
	}
}

void Orb::drawTrail() {
   //if (!(_trail)) return;
	glPushMatrix();
	// orb rotation
	pnt3d def = pnt3d(0.0, 0.0, -1.0);
	pnt3d path = pnt3d(camera.pos.x, camera.pos.y, camera.pos.z) -
						pnt3d(0,0,1);
	path = path.normalize(path);
	pnt3d cross = def.crossproduct(path);
	float radians = def.dotproduct(path);
	radians = acos(radians/(def.mag()*path.mag()));
	radians = radians*(180/M_PI);

	glTranslatef(_trail->_pos.x, _trail->_pos.y, _trail->_pos.z);
	glRotatef(radians, cross.x, cross.y, cross.z);

	glEnable (GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glBindTexture(GL_TEXTURE_2D, lightsourcetexture);


	float width = 0.30 * ((float)_trail->_length / 20);

	if(!completed) {
		glBegin(GL_POLYGON);
		glColor3f(1.0, 1.0, 1.0); //white
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-width, width, 0.0); //v5
		glTexCoord2f(0.0, 1.0);
		glVertex3f(width, width, 0.0); //v6
		glTexCoord2f(1.0, 1.0);
		glVertex3f(width, -width, 0.0); //v7
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-width, -width, 0.0); //v8
		glEnd();
	}

	glDisable (GL_BLEND);
	glPopMatrix();

	if(_trail->_length > 0) {
		_trail->drawTrail();
	}
}

pnt3d Source::collisionAvoidance(Orb o) {
	pnt3d c;

	
	for(int i = 0; i < _orbcount; i++) {
		if(_orbList[i]._active && _orbList[i]._orbId != o._orbId) {
			if(_orbList[i]._pos.distBetween(o._pos) < 0.5) {
				if(_orbList[i]._pos.distBetween(_pos) < 200) {
					c = c - (_orbList[i]._pos - o._pos);
				}
			}
		}
	}
	

	return c;
}

pnt3d tend_to_place(Orb o, pnt3d dest, float force) {
	return (dest - o._pos) / 10;
}

pnt3d limit_velocity(Orb o) {
	float vlim = 0.30; // This is the max velocity

	if(o._velocity.mag() > vlim) {
		o._velocity = (o._velocity / o._velocity.mag()) * vlim;
	}
	
	return o._velocity;
}


//bool Source::hitTest(float ix,float iy,float iz) { return false; }
//pnt3d Source::hitSurface(float ix,float iy,float iz) { return pnt3d(); }
//bool Orb::hitTest(float ix,float iy,float iz) { return false; }
//pnt3d Orb::hitSurface(float ix,float iy,float iz) { return pnt3d(); }
