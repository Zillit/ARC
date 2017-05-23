#version 150

out vec4 outColor;
in vec3 exNormal; // Phong

void main(void)
{
	const vec3 light = vec3(0.78, 0.38, 0.78);
	float shade;
	
	shade = dot(normalize(exNormal), light);
	// outColor = vec4(light,0);
	outColor = vec4(shade*0.8, shade*0.5, shade*0.7, 1.0);
}