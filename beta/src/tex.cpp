/* Texture Functions
 * Provided by Zoe Wood F2010 */

#include "tex.h"

int size = 0;
char texturenames[5012][128];
//std::vector<char *> texturenames;

void materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}

int exists(char* imageFile){
   /* Starts at 1 because blur is 0 */
  for(int i = 0; i < size; i++){
    if(!strcmp(texturenames[i], imageFile)){
      return i + 1;
    }
  }
  return -1;
}

GLuint LoadTexture(char* image_file, bool *doesExist) {
  //printf("loading %s\n", image_file);
  int r = exists(image_file);
  if(r != -1){
    *doesExist = true;
    return r;
  }
  //r = SOIL_load_OGL_texture(image_file, 0, 0, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS);
  //Faster
  r = SOIL_load_OGL_texture(image_file, 0, 0, 0);
  if(r == 0){
     printf("Error loading: %s\n", image_file);
  }
  else{
     strcpy(texturenames[size], image_file);
     //texturenames.push_back(image_file);
     size++;
  }
  return r;
}

GLuint LoadTexture(char* image_file) {
  //printf("loading %s\n", image_file);
  int r = exists(image_file);
  if(r != -1){
    return r;
  }
  //r = SOIL_load_OGL_texture(image_file, 0, 0, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS);
  //Faster
  r = SOIL_load_OGL_texture(image_file, 0, 0, 0);
  if(r == 0){
     printf("Error loading: %s\n", image_file);
  }
  else{
     strcpy(texturenames[size], image_file);
     //texturenames.push_back(image_file);
     size++;
  }
  return r;
}

void LoadExternTexture(char* image_file) {
     strcpy(texturenames[size], image_file);
     //texturenames.push_back(image_file);
     size++;
}

char* getTexName(int t){
	return texturenames[t - 1];
}

void removeAllTextures() {
   GLuint array[size];
   for (int i = 0; i < size-1; i++) {
      array[i] = i+1;
      strcpy(texturenames[i], "");
   }
   glDeleteTextures(size-1, array);
   size = 1;
}
