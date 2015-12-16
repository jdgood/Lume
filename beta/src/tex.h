/* Texture functions 
 * Provided by Zoe Wood F2010*/

#if ! defined _TEX_H
#define _TEX_H 1

#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <vector>
#include "SOIL.h"

typedef struct Image {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
} Image;

typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;

int ImageLoad(char *filename, Image *image);
GLuint LoadTexture(char* image_file, bool *doesExist);
GLuint LoadTexture(char* image_file);
void materials(materialStruct materials);
void LoadExternTexture(char* image_file);
char* getTexName(int t);
void removeAllTextures();

/*static materialStruct mNone = {
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0}
};

static materialStruct mBlack = {
  {0.2, 0.2, 0.2, 1.0},
  {0.2, 0.2, 0.2, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0}
};
static materialStruct mWhite = {
  {1, 1, 1, 1.0},
  {1, 1, 1, 1.0},
  {1, 1, 1, 1.0},
  {10}
};
static materialStruct mGold = {
  {0.24725, 0.1995, 0.0745, 1.0},
  {0.75164, 0.60648, 0.22648, 1.0},
  {0.628281, 0.555802, 0.366065, 1.0},
  {60}
};
static materialStruct mGrey = {
  {0.4, 0.4, 0.4, 1.0},
  {0.6, 0.6, 0.6, 1.0},
  {0.6, 0.6, 0.6, 1.0},
  {0.0}
};*/

#endif
