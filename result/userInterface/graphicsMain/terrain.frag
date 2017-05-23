#version 150

out vec4 outColor;
in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fSurface;
in vec3 fPosition;
uniform sampler2D tex;
uniform mat4 camMatrix;
// Get the lighting
// uniform vec3 lightSourcesDirPosArr[4];
// uniform vec3 lightSourcesColorArr[4];
// uniform float specularExponent[4];
// uniform bool isDirectional[4];

void main(void)
{
	vec3 ambient = vec3(0.5,0.2,0.2);
	vec3 surfaceToCamera = normalize(vec3(camMatrix)-fSurface);
	vec4 sidewaysLight=vec4(0.0,0.4,0.0,0.0);
	vec4 sidewaysLightDirection=vec4(0.5,0.5,-0.5,0.0);
	vec4 sidewaysLightPosition=vec4(-0.5,-0.5,0.5,0.0);
	float diffuseCoefficient=max(0.0,dot(fNormal,normalize(sidewaysLightPosition.xyz)));
	vec4 diffuseSideway=diffuseCoefficient*(texture(tex, fTexCoord))*sidewaysLight;
	vec4 centerLight=vec4(0,0.5,0.9,0.0);
	vec4 centerLightPosition=vec4(0.0,100.0,-64.0,0.0);
	vec4 centerLightDirection=vec4(1.0,-1.0,0.5,0.0);
	centerLightDirection=normalize(centerLightDirection);
	vec3 surfaceToLight=normalize(centerLightPosition.xzy-fPosition);
	float distanceToCenterLight=length(centerLightPosition.xyz-fPosition)/50;
	float attenuation=1.0/(1.0+pow(distanceToCenterLight,2));
	float diffuseCenterCoefficient=max(0.0,dot(fNormal,surfaceToLight));
	
	vec4 diffuseCenter=(texture(tex, fTexCoord))*attenuation*centerLight*pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, fNormal))),1);

	vec3 nP=normalize(fPosition)*0.5;
	outColor = clamp(diffuseSideway + diffuseCenter + vec4(ambient,0)*(texture(tex, fTexCoord))+vec4(nP.xyz,0),0,1);
}
