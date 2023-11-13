#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

const float scaleFactors[7] = float[](0.006 , 0.061 , 0.242 , 0.383 , 0.242 , 0.061 , 0.006);

void main(void)
{
	fragColor = vec4(0, 0, 0, 1);
	
	vec4 diffuse = texture2D(sceneTex, IN.texCoord);
	float brightness = dot(diffuse.rgb, vec3(0.9126, 0.7152, 0.0722));

    if(brightness > 1.0)
	{
        diffuse = vec4(diffuse.rgb, 1.0);
	}
    else
	{
        diffuse = vec4(0.0, 0.0, 0.0, 1.0);
	}
	
	fragColor =  diffuse;
}