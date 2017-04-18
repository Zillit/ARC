#version 150

out vec4 outColor;
in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fSurface;
in vec3 fPosition;
uniform sampler2D tex;

// Get the lighting
// uniform vec3 lightSourcesDirPosArr[4];
// uniform vec3 lightSourcesColorArr[4];
// uniform float specularExponent[4];
// uniform bool isDirectional[4];

void main(void)
{
	vec3 ambient = vec3(0.4,0.2,0.2);
	// const vec3 vlight = vec3(0.58, 0.58, 0.58);
	// float totlight, diffuse, phong, amoutDiffuse, shade;

	// phong = 1;

	// diffuse = dot(normalize(fNormal),vlight);
	// diffuse = max(0.0, diffuse);

	// // Specular
	// vec3 r = reflect(-vlight, normalize(fNormal));
	// vec3 v = normalize(-fSurface); // View direction
	// phong = dot(r, v);
	// if (phong > 0.0)
	// 	phong = 1.0 * pow(phong, 500.0);
	// phong = max(phong, 0.0);

	// amoutDiffuse = 0.7;
	// totlight = amoutDiffuse * diffuse + (1.0 - amoutDiffuse) * phong;

	// vec3 storeLight[4]; // To store the four different lights here to sum them up lateron
	// for (int i=0; i < 4; i++) {
	// 	vec3 vlight = vec3(lightSourcesDirPosArr[i]);
	// 	float totlight, diffuse, phong, amoutDiffuse;
	// 	vec3 r;

	// 	// Diffuse
	// 	diffuse = dot(normalize(fNormal),vlight);
	// 	diffuse = max(0.0, diffuse);

		
	// 	if (isDirectional[i] == true) {
	// 	// Directional
	// 		r = reflect(-vlight, normalize(fNormal));
	// 	} else {
	// 	// Positional
	// 		r = reflect(-(vlight-fSurface), normalize(fNormal));
	// 	}
	// 	vec3 v = normalize(-fSurface); // View direction
	// 	phong = dot(normalize(r), v);
	// 	if (phong > 0.0)
	// 		phong = 1.0 * pow(phong, specularExponent[i]);
	// 	phong = max(phong, 0.0);

	// 	amoutDiffuse = 0.7;
	// 	totlight = amoutDiffuse * diffuse + (1.0 - amoutDiffuse) * phong;
		
	// 	storeLight[i] = lightSourcesColorArr[i]*totlight;
	// }
	// // outColor = totlight*(texture(tex, fTexCoord));

	// vec3 mixLight = ambient;
	// for (int j=0; j<4; j++) {
	// 	mixLight = mixLight + storeLight[j];
	// }
	// mixLight = mixLight / 4;
	// vec4 totallight = texture(tex, fTexCoord);
	// 	outColor = vec4(totallight[0] * mixLight[0],totallight[1] * mixLight[1],totallight[2] * mixLight[2],0.0)+vec4(0.1,0.1,0.1,0);
	vec3 nP=normalize(fPosition);
	outColor = clamp((texture(tex, fTexCoord)+vec4(ambient,0)+vec4(nP.xyz,0)),0,1);
}
