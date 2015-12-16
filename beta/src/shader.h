#if ! defined _SHADER_H
#define _SHADER_H 1

#include "util.h"

class Shader{
  public:
     Shader(GLuint one, GLuint two){texid = one; texid2 = two;}
     ~Shader(){}
     GLuint texid, texid2;
     GLuint frag;
     GLuint loadFrag(const char* fragfile);
     void bind();
     void unbind();
};

//GLuint p,p2;
  //p = loadFragShader("shaders/lights.frag"); // p2 = loadFragShader("shaders/bg.frag");

#endif
