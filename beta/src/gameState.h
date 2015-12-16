#if ! defined _GAMESTATE_H
#define _GAMESTATE_H 1

#include "importMap.h"

class gameState{
  public:
     gameState(){};
     virtual ~gameState(){}
     void save(char* SaveName);
     int load(char* SaveName);
};

#endif
