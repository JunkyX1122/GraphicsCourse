#include "Renderer.h"

void Renderer::UpdatePointLights(float dt)
{
	pointLightCycle += dt * 0.25f;
	for (int i = 0; i < pointLightPositions.size(); i++)
	{
		Light& l = pointLights[i];
		float rad = 3000.0f + 500.0f * sin(2 * PI * pointLightCycle + pointLightStarts[i]);
		l.SetRadius(rad);
		l.SetPosition(pointLightPositions[i] - Vector3(rad, rad, rad) / 4);
	}
}

void Renderer::UpdateCameraMovement(float dt)
{
	cameraTimer += dt/8;
	if (cameraTimer >= 1)
	{
		cameraTimer = 0;
		currentKeyFrame = (currentKeyFrame + 1) % cameraKeyFrameCount_Planet;
		std::cout << currentKeyFrame << "\n";
	}
	Vector3 pos1 = cameraPositions_Planet[currentKeyFrame];
	Vector3 pos2 = cameraPositions_Planet[(currentKeyFrame + 1) % cameraKeyFrameCount_Planet];
	Vector3 posTrans = Vector3(
		naive_lerp(pos1.x, pos2.x, cameraTimer),
		naive_lerp(pos1.y, pos2.y, cameraTimer),
		naive_lerp(pos1.z, pos2.z, cameraTimer)
	);
	camera->SetPositionSetter(posTrans);

	Vector3 rot1 = cameraRotations_Planet[currentKeyFrame];
	Vector3 rot2 = cameraRotations_Planet[(currentKeyFrame + 1) % cameraKeyFrameCount_Planet];
	Vector3 rotTrans = Vector3(
		naive_lerp(rot1.x, rot2.x, cameraTimer),
		naive_lerp(rot1.y, rot2.y, cameraTimer),
		naive_lerp(rot1.z, rot2.z, cameraTimer)
	);
	camera->SetRotationSetter(rotTrans);
}

float Renderer::naive_lerp(float a, float b, float t)
{
	return a + t * (b - a);
}
