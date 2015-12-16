uniform sampler2D bgl_RenderedTexture;		

void main()
{
	// Sampling The Texture And Passing It To The Frame Buffer
   vec4 clr = gl_Color*texture2D(bgl_RenderedTexture, gl_TexCoord[0].st);
   clr.x = gl_FragCoord.x/1000;
   clr.y = gl_FragCoord.y/1000;
   clr.z = gl_FragCoord.z/1000;
	gl_FragColor = clr;
}

