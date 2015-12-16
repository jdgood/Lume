#include "model.h"


// typedef struct Image {
//   unsigned long sizeX;
//   unsigned long sizeY;
//   char *data;
// } Image;

Image *TextureImage;

typedef struct RGB {
  GLubyte r;
  GLubyte g; 
  GLubyte b;
} RGB;

RGB myimage[64][64];
RGB* pixel;

model m = model();

/* BMP file loader loads a 24-bit bmp file only */

/*
* getint and getshort are help functions to load the bitmap byte by byte
*/
// static unsigned int getint(FILE *fp) {
//   int c, c1, c2, c3;
  
//   /*  get 4 bytes */ 
//   c = getc(fp);  
//   c1 = getc(fp);  
//   c2 = getc(fp);  
//   c3 = getc(fp);
  
//   return ((unsigned int) c) +   
//     (((unsigned int) c1) << 8) + 
//     (((unsigned int) c2) << 16) +
//     (((unsigned int) c3) << 24);
// }

// static unsigned int getshort(FILE *fp){
//   int c, c1;
  
//   /* get 2 bytes*/
//   c = getc(fp);  
//   c1 = getc(fp);
  
//   return ((unsigned int) c) + (((unsigned int) c1) << 8);
// }

// int ImageLoad(char *filename, Image *image) {
//   FILE *file;
//   unsigned long size;                 /*  size of the image in bytes. */
//   unsigned long i;                    /*  standard counter. */
//   unsigned short int planes;          /*  number of planes in image (must be 1)  */
//   unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
//   char temp;                          /*  used to convert bgr to rgb color. */
  
//   /*  make sure the file is there. */
//   if ((file = fopen(filename, "rb"))==NULL) {
//     printf("File Not Found : %s\n",filename);
//     return 0;
//   }
  
//   /*  seek through the bmp header, up to the width height: */
//   fseek(file, 18, SEEK_CUR);
  
//   /*  No 100% errorchecking anymore!!! */
  
//   /*  read the width */    
//   image->sizeX = getint(file);
  
//   /*  read the height */ 
//   image->sizeY = getint(file);
  
//   /*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
//   size = image->sizeX * image->sizeY * 3;
  
//   /*  read the planes */    
//   planes = getshort(file);
//   if (planes != 1) {
//     printf("Planes from %s is not 1: %u\n", filename, planes);
//     return 0;
//   }
  
//   /*  read the bpp */    
//   bpp = getshort(file);
//   if (bpp != 24) {
//     printf("Bpp from %s is not 24: %u\n", filename, bpp);
//     return 0;
//   }
  
//   /*  seek past the rest of the bitmap header. */
//   fseek(file, 24, SEEK_CUR);
  
//   /*  read the data.  */
//   image->data = (char *) malloc(size);
//   if (image->data == NULL) {
//     printf("Error allocating memory for color-corrected image data");
//     return 0; 
//   }
  
//   if ((i = fread(image->data, size, 1, file)) != 1) {
//     printf("Error reading image data from %s.\n", filename);
//     return 0;
//   }
  
//   for (i=0;i<size;i+=3) { /*  reverse all of the colors. (bgr -> rgb) */
//     temp = image->data[i];
//     image->data[i] = image->data[i+2];
//     image->data[i+2] = temp;
//   }
  
//   fclose(file); /* Close the file and release the filedes */
  
//   /*  we're done. */
//   return 1;
// }

// //routines to load in a bmp files - must be 2^nx2^m and a 24bit bmp
// GLvoid LoadTexture(char* image_file, int texID) { 
  
//    TextureImage = (Image *) malloc(sizeof(Image));
//    if (TextureImage == NULL) {
//       printf("Error allocating space for image");
//       exit(1);
//    }
//    // cout << "trying to load " << image_file << endl;
//    if (!ImageLoad(image_file, TextureImage)) {
//       exit(1);
//    }  
//    /*  2d texture, level of detail 0 (normal), 3 components (red, green, blue),  */
//    /*  x size from image, y size from image,                                     */    
//    /*  border 0 (normal), rgb color data, unsigned byte data, data               */ 
//    glBindTexture(GL_TEXTURE_2D, texID);
//    glTexImage2D(GL_TEXTURE_2D, 0, 3,
//                 TextureImage->sizeX, TextureImage->sizeY,
//                 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
//    /*  cheap scaling when image bigger than texture */    
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//    /*  cheap scaling when image smalled than texture*/
//    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
// }

void
reshape (int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport (0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, w/(GLdouble)h, 0.1, 1000.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}

void
display ()
{
  int i;
  static float angle = 0;
  static double curent_time = 0;
  static double last_time = 0;

  last_time = curent_time;
  curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();
  m.draw(0, m.texID);

  glutSwapBuffers ();
  glutPostRedisplay ();
}

void
keyboard (unsigned char key, int x, int y)
{
  /* Escape */
  if (key == 27)
    exit (0);
}

//general intialization for opengl for depth and texture mapping
// void init() {
//   glEnable(GL_DEPTH_TEST);
//   glEnable(GL_TEXTURE_2D);
// }

//initialization for texture mapping
// void init_tex() {
//   glBindTexture(GL_TEXTURE_2D, 0);
//   glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// }

void
cleanup ()
{
   FreeModel (&(m.md5file));
   // FreeAnim (&(m.md5anim));

  if (m.animated && m.skeleton)
    {
      free (m.skeleton);
      m.skeleton = NULL;
    }

  m.FreeVertexArrays ();
}

int
main (int argc, char *argv[])
{
  /*if (argc < 2)
    {
      fprintf (stderr, "usage: %s <filename.md5mesh> "
	       "[<filename.md5anim>]\n", argv[0]);
      return 0;
    }*/

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (640, 480);
  glutCreateWindow ("MD5 Model");

  atexit (cleanup);
  m.init ("guy.md5mesh", NULL);

  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyboard);

  // load in image textures
  m.texID = LoadTexture(m.md5file.meshes->shader);
  printf("loading texture %s\n", m.md5file.meshes->shader);

  glutMainLoop ();

  return 0;
}
