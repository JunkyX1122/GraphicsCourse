#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[2];
/*
void main(void)
{
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	vec3 normal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;

	fragColour[0] = texture2D(diffuseTex, IN.texCoord);
	fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}
*/

void main(void)
{
	vec3 incident = normalize(lightPos - IN.worldPos);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	vec3 normal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	float lambert = max(dot(incident, bumpNormal), 0.0f);
	float distance = length(lightPos - IN.worldPos);
	float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

	float specFactor = clamp(dot(halfDir, bumpNormal), 0.0, 1.0);
	specFactor = pow(specFactor, 120.0);

	vec3 surface = (diffuse.rgb * lightColour.rgb);

	fragColour[0].rgb = surface * lambert * attenuation;
	fragColour[0].rgb += (lightColour.rgb * specFactor) * attenuation * 0.33;
	fragColour[0].rgb += surface * 0.1f;
	fragColour[0].a = diffuse.a;

	fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}