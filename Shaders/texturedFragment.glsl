#version 330 core

uniform sampler2D diffuseTex;
uniform float timer;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	fragColour = texture(diffuseTex, IN.texCoord);
	//fragColour.x += 0.5*sin(timer);
	//fragColour.y += 0.5*cos(timer);
	//fragColour.z += 0.5*sin(timer * 2.0f);
}