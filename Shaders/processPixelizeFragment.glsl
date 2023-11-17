#version 330 core

uniform sampler2D sceneTex;

uniform vec2 screenSize;
uniform float pixelSize;
uniform vec4 colourCorrection;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{
	vec2 newUV = floor(IN.texCoord.xy * screenSize / pixelSize) / screenSize * pixelSize;
	if(floor(pixelSize)<=1)
	{
		newUV = IN.texCoord.xy;
	}
	vec4 tmp = texture2D(sceneTex, newUV);

	fragColor = tmp;
	fragColor = fragColor * colourCorrection;
	
}
