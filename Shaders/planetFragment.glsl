#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex; 
uniform sampler2D diffuseTex2; 

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

uniform float timer;
uniform vec4 transformValues;
uniform int lightInfluence;
in Vertex 
{
	vec3 colour;
	vec4 texCoord;
	vec3 normal;
	vec3 tangent; 
	vec3 binormal; 
	vec3 worldPos;
} IN;

out vec4 fragColour ;
void main(void)
{
	
	vec3 incident = normalize(lightPos - IN.worldPos);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	vec2 longitudeLatitude = vec2((atan(IN.texCoord.y, IN.texCoord.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(IN.texCoord.z) / 3.1415926 + 0.5));

	//float timerAlt = timer * 0.1f * 0.25f * 0.25f;
	vec2 texCoordsTransformed = (longitudeLatitude * transformValues.xy) + transformValues.zw;



	vec4 diffuse = texture(diffuseTex, longitudeLatitude);
	vec4 diffuse2 = texture(diffuseTex2, texCoordsTransformed);
	vec3 bumpNormal = texture(bumpTex, longitudeLatitude).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	float lambert = max(dot(incident, bumpNormal), 0.0f);
	float distance = length(lightPos - IN.worldPos);
	float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

	float specFactor = clamp(dot(halfDir, bumpNormal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0);

	vec3 surface = ((diffuse.rgb + diffuse2.rgb * 0.5));
	surface = lightInfluence == 1 ? surface * lightColour.rgb : surface;
	fragColour.rgb = surface;
	if(lightInfluence==1)
	{
		fragColour.rgb = fragColour.rgb * lambert * attenuation;
		fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.33;
		fragColour.rgb += surface * 0.1f;
	}
	fragColour.a = diffuse.a;
}