#include "Renderer.h"

void Renderer::UpdatePointLights(float dt)
{
	pointLightCycle += dt * 0.25f;
	for (int i = 0; i < pointLightPositions.size(); i++)
	{
		Light& l = pointLights[i];
		float rad = 2000.0f + 500.0f * sin(2 * PI * pointLightCycle + pointLightStarts[i]);
		l.SetRadius(rad);
		l.SetPosition(pointLightPositions[i] - Vector3(rad, rad, rad) / 4);
	}
}