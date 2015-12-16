#include "FBO.h"

FBO::FBO()
{	
   state = 0;
   blurAmount = BLUR_AMOUNT;
   blurIntensity = BLUR_INTENSITY;
   blurDecay = BLUR_DECAY;
   blurOffset = BLUR_OFFSET;
   width = GW;
   height = GH;
   
   
   //frame
	glGenFramebuffersEXT(1, &(frame));
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame);

   //depth
	glGenRenderbuffersEXT(1, &(depth));
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT/*GL_DEPTH_COMPONENT*/, width, height);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth);

	glGenTextures(1, &(texid));
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texid, 0);

   glGenTextures(1, &(texid2));
	glBindTexture(GL_TEXTURE_2D, texid2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, texid2, 0);

   /*glGenTextures(1, &(texid3));
	glBindTexture(GL_TEXTURE_2D, texid3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, texid3, 0);*/

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		printf("FBO Error: %d\n", status);
	   //exit(-1);
	}
}

FBO::~FBO()
{
	glDeleteTextures(1, &(texid));
   glDeleteTextures(1, &(texid2));
   /*glDeleteTextures(1, &(texid3));*/
	glDeleteRenderbuffersEXT(1, &(depth));
	glDeleteFramebuffersEXT(1, &(frame));
}

void FBO::resize()
{
   width = GW;
   height = GH;
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT/*GL_DEPTH_COMPONENT*/, width, height);
   glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glBindTexture(GL_TEXTURE_2D, texid2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   /*glBindTexture(GL_TEXTURE_2D, texid3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);*/
}

void FBO::smallflash(){
   state = 2;
   blurAmount = 10;
   blurIntensity = 4;
   blurOffset = 0.01;
}

void FBO::flash(){
   state = 1;
   blurAmount = 10;
   blurIntensity = 8;
   blurOffset = 0.04;
}

void FBO::update(float dt){
   if (state){
      blurIntensity-=.04*dt;
      blurOffset-=.0002*dt;
      if (blurOffset <= 0 || blurIntensity <= BLUR_INTENSITY){
         blurAmount = BLUR_AMOUNT;
         blurIntensity = BLUR_INTENSITY;
         blurOffset = BLUR_OFFSET;
         state = 0;
      }
   }
}

void FBO::glowEdges(){
   glBindTexture(GL_TEXTURE_2D, texid2);
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

   float gblurAmount = EDGE_AMOUNT;
   float gblurIntensity = EDGE_INTENSITY;
   float gblurOffset = EDGE_OFFSET;

   if (BLUR_ON) {  
      gblurAmount = EDGE_AMOUNT;
      gblurIntensity = EDGE_INTENSITY;
      gblurOffset = EDGE_OFFSET;
   } else {
      gblurAmount = 1;
      gblurIntensity = 1;
      gblurOffset = 0;
   }

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float offset = 0.0f;										
   float alpha = ( (blurDecay*(gblurAmount+1)/2.0f) + (1.0f/gblurAmount) ) * gblurIntensity/2;//optimized for center to be ~1 
   float alphainc = blurDecay;// exponential edges
   if (BLOOM_ON) bloom->bind();
	glBegin(GL_QUADS);
   for (int i = 0;i < gblurAmount;i++)// number of times to render blur
	{
		glColor4f(1.0f, 1.0f, 1.0f, alpha);	
		glTexCoord2f(0.0f+offset, 0.0f+offset);
		glVertex2d(0, 0);
		glTexCoord2f(1.0f-offset, 0.0f+offset);
		glVertex2d(GW/GH, 0);
		glTexCoord2f(1.0f-offset, 1.0f-offset);
		glVertex2d(GW/GH, 1);
		glTexCoord2f(0.0f+offset, 1.0f-offset);
		glVertex2d(0, 1);
		offset += gblurOffset;
		alpha = alpha - alphainc;					
	}
	glEnd();
   if (BLOOM_ON) bloom->unbind();
   /*// add third view
   glBindTexture(GL_TEXTURE_2D, texid3);
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 1);	
		glTexCoord2f(0.6f, 0.6f);
		glVertex2d(0, 0);
		glTexCoord2f(1.0f, 0.6f);
		glVertex2d(GW/GH, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2d(GW/GH, 1);
		glTexCoord2f(0.6f, 1.0f);
		glVertex2d(0, 1);
	glEnd();*/
}

void FBO::draw(){
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   glDisable(GL_DEPTH_TEST);									
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);			
	glEnable(GL_BLEND);
  
   //glUseProgram(frag);
   //int loc = glGetUniformLocation(frag, "fbo_tex");
   
   //glActiveTexture(texid);	
   glBindTexture(GL_TEXTURE_2D, texid);

   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

   //glUniform1i(loc, texid);
   
   //glUseProgram(p2);
   //GLuint loc = glGetUniformLocation(p, "fbotex");
   //glUniform1i(loc, texid);


   //glUniform1i(getUniLoc(p, "tex0"), 0);
	//loc = glGetUniformLocation(sh.p[1], "Width");
	//glUniform1i(loc, width);
	//loc = glGetUniformLocation(sh.p[1], "Height");
	//glUniform1i(loc, height);
	//loc = glGetUniformLocation(sh.p[1], "Dist");
	//glUniform1i(loc, dist);
	//loc = glGetUniformLocation(sh.p[1], "Kernel");
	//glUniformMatrix3fv(loc, 1, GL_FALSE, &kernels[curker]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float offset = 0.0f;										
   float alpha = ( (blurDecay*(blurAmount+1)/2.0f) + (1.0f/blurAmount) ) * blurIntensity;//optimized for center to be ~1 
   float alphainc = blurDecay;// exponential edges

   //pixeld->bind();
	glBegin(GL_QUADS);
   for (int i = 0;i < blurAmount;i++)// number of times to render blur
	{
		glColor4f(1.0f, 1.0f, 1.0f, alpha);	
		glTexCoord2f(0.0f+offset, 0.0f+offset);
		glVertex2d(0, 0);
		glTexCoord2f(1.0f-offset, 0.0f+offset);
		glVertex2d(GW/GH, 0);
		glTexCoord2f(1.0f-offset, 1.0f-offset);
		glVertex2d(GW/GH, 1);
		glTexCoord2f(0.0f+offset, 1.0f-offset);
		glVertex2d(0, 1);
		offset += blurOffset;
		alpha = alpha - alphainc;					
	}
	glEnd();
   //pixeld->unbind();

   glowEdges();
   
   //glUseProgram(-1);
   //glUseProgram(p);
   //GLuint loc = glGetUniformLocation(p, "fbo");
   //glUniform1i(loc, texid);

   //glEnable(GL_LIGHTING);  
   glEnable(GL_DEPTH_TEST);									/* Enable Depth Testing */
	glDisable(GL_BLEND);
}
