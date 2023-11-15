#version 330 core

uniform sampler2D sceneTexBase;
uniform sampler2D sceneTex;


in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{
	vec4 baseFrag = texture2D(sceneTexBase, IN.texCoord.xy);
	
	vec4 bloomFrag = texture2D(sceneTex, IN.texCoord.xy);

	fragColor = baseFrag + bloomFrag;
}