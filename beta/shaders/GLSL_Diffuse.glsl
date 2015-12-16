//GLSL shader which compute diffuse shading
//light position set in DiffuseShader.cpp
//ZJW 2/2010

//global variable that rarely change
uniform vec3 LightPos;

void main()
{

	// transform the vertex into world space- required
	vec4 pos = gl_ModelViewMatrix*gl_Vertex;
	
	//xform normal from model-space to view-space
	vec3 normalVec = normalize(gl_NormalMatrix*gl_Normal);

	//has no camera transform on light fine for this example
	vec3 lightVec = normalize(vec3(LightPos.x-pos.x, LightPos.y-pos.y, LightPos.z-pos.z));

	//calculate the diffuse contribution
	float diffuse = max(dot(lightVec, normalVec), 0.0);

	//blue diffuse material
	vec4 diffuseMaterial = vec4(0.2, 0.2, 0.8, 1.0);

	//combine terms to make color
	gl_FrontColor = diffuse*diffuseMaterial + 0.3*diffuseMaterial;

	//full transform required
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	
}
