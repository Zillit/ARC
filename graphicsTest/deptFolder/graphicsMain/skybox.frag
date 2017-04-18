#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;

void main(void)
{
	outColor = vec4(0.2,0.2,0.4,0.0);//texture(tex, texCoord);
}
