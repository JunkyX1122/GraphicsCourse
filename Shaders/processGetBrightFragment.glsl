#version 330 core

uniform sampler2D sceneTex;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColor;

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

	/*
	

	vec2 delta = vec2(0, 0);
	for(int pass2 = 0; pass2 < 20; pass2++)
	{
		if(pass2%2 == 0)
		{
			delta = dFdy(IN.texCoord);
		}
		else
		{
			delta = dFdx(IN.texCoord);
		}

		for(int i = 0; i < 7; i++)
		{
			vec2 offset = delta * (i - 3);
			vec4 tmp = texture2D(sceneTex, IN.texCoord.xy + offset);
			fragColor += tmp * scaleFactors[i];
		}
	}
	*/
}