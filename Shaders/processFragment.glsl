#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;
uniform int bloomRange;
uniform float bloomIntensity;
in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

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
	float z = 0;
    int ranger = bloomRange;
	float startCurve = 8.0f;
	for(int i = 0; i < ranger; i++)
	{
		vec2 offset = delta * (z - (ranger-1)/2);
		vec4 tmp = texture2D(sceneTex, IN.texCoord.xy + offset);
		
		float curve = 1.0/(pow(2.0, 
		abs(

		-startCurve + (startCurve*2)/(ranger-1) * z
		
		)
		));
		fragColor += tmp * curve * bloomIntensity;
		z+=1.0;
	}
}

