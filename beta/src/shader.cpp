#include "shader.h"

char *textFileRead(const char *fn) 
{
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {

   if ((fp = fopen(fn, "rb"))==NULL) {
      printf("File Not Found : %s\n",fn);
      return 0;
   }
		if (fp != NULL) {
									      
     	      		fseek(fp, 0, SEEK_END);
     			count = ftell(fp);
     			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
									
		}
	}

	return content;
}

GLuint Shader::loadFrag(const char* fragfile)
{	
	char *fs;
   GLuint p;
   GLuint f;
	f = glCreateShader(GL_FRAGMENT_SHADER);	
	fs = textFileRead(fragfile);

	const char * ff = fs;
	glShaderSource(f, 1, &ff,NULL);

   free(fs);

	glCompileShader(f);	
	p = glCreateProgram();
	GLsizei len;
	GLchar buffer[1024];
	glGetShaderInfoLog(p, 1024, &len, buffer);
	if(len > 0){
   	printf("%s\n", buffer);
   }
	glAttachShader(p,f);	
	glLinkProgram(p);
   frag = p;
   return p;
}

void Shader::bind() {
	
   int loc = glGetUniformLocation(frag, "bgl_RenderedTexture");

   glActiveTexture(texid2);	
   glBindTexture(GL_TEXTURE_2D, texid2);

   glUniform1i(loc, texid2);
   glUseProgram(frag);
}

void Shader::unbind() {
	glUseProgram(0);
}
