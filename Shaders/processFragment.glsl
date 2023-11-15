#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

const float scaleFactors[7] = float[](0.006 , 0.061 , 0.242 , 0.383 , 0.242 , 0.061 , 0.006);

float pdf(float x, float  mu=0.0f, float  sigma=1.0f)
{
    x = (x - mu) / sigma;
    return (exp(-x*x/2.0f) / sqrt(2.0f*3.14159f) / sigma);
}


void main(void)
{
	fragColor = vec4(0, 0, 0, 1);
	vec2 delta = vec2(0, 0);

	if(isVertical == 1)
	{
		delta = dFdy(IN.texCoord);
	}
	else
	{
		delta = dFdx(IN.texCoord);
	}
	int z = 0;
    float border = 4;
    float ranger = 7;
	for(int i = 0; i < ranger; i++)
	{
		vec2 offset = delta * (i - 3);
		vec4 tmp = texture2D(sceneTex, IN.texCoord.xy + offset);
		
		
		fragColor += tmp * scaleFactors[i] ;
		z+=1;
	}
}

