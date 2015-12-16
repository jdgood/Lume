//GLSL shader which compute diffuse shading
//light position set in DiffuseShader.cpp
//ZJW 2/2010

//global variable that rarely change
varying vec3 normal;
varying vec3 vertex_to_light;

void main()
{
	//normalize the interpolated normal and light
	vec3 normalVec = normalize(normal);

	//has no camera transform on light fine for this example
	vec3 lightVec = normalize(vertex_to_light);

	//calculate the diffuse contribution
	float diffuse = clamp(dot(lightVec, normalVec), 0.0, 1.0);


	vec3 eye = normalize(vec3(0.0, 0.0, 4.0));
	vec3 hdir = (eye + lightVec)/2.0;//cross(eye, lightVec);

	float spec = clamp(pow(dot(normalVec, normalize(hdir)), 50.0), 0.0, 1.0);

	//blue diffuse material
	const vec4 diffuseMaterial = vec4(0.2, 0.2, 0.8, 1.0);

	const vec4 specularMaterial = vec4(1.0, 1.0, 1.0, 1.0);

	//combine terms to make color
	gl_FragColor = diffuse*diffuseMaterial + spec*specularMaterial + 0.3*diffuseMaterial;

	/*now add a specular contribution */	
	
}
