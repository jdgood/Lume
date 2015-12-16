#if ! defined _Tree_H
#define _Tree_H 1

#include "model.h"
#include "obj.h"
#include "player.h"

extern Player character;

class Tree : public model {
	public:
		// animation states
		bool growing;

		// constructor
		Tree();
		Tree(float x, float y, float z);
      bool hitCollision(obj object);
		// overwrite draw
		void draw();
};

#endif
