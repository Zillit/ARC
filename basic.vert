#version 150
// uniform mat4 projMatrix;
// uniform mat4 mdlMatrix;
// uniform mat4 cameraMatrix;


// // in  vec3 inPosition;
// // in  vec3 inNormal;
// // in vec2 inTexCoord;
// out vec2 fTexCoord;
// out vec3 fSurface;
// out vec3 fNormal;


void main(void)
{
	// fNormal = mat3(mdlMatrix)*inNormal;
	// fTexCoord = inTexCoord;
	// fSurface = vec3(mdlMatrix * vec4(inPosition, 1.0));
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
}
