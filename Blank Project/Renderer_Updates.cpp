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

void Renderer::UpdateCameraControls()
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))
	{
		cameraAnimateSpeed += 0.001f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS))
	{
		cameraAnimateSpeed -= 0.001f;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT))
	{
		cameraAnimateSpeed = 1.0f / 8.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT))
	{
		cameraAnimateSpeed = -1.0f / 8.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M))
	{
		if (camera->GetFreeMove())
		{
			camera->LockFreeMovement();
		}
		else
		{
			camera->UnlockFreeMovement();
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_N))
	{
		std::cout << "Current Cam Keyframe: " << currentKeyFrame << "\n";
		camera->SetPositionSetter(cameraPositions_Planet[currentKeyFrame]);
		camera->SetRotationSetter(cameraRotations_Planet[currentKeyFrame]);
		currentKeyFrame = (currentKeyFrame + 1) % cameraKeyFrameCount_Planet;
	}
}

void Renderer::UpdateCameraMovement(float dt)
{
	cameraTimer += dt * cameraAnimateSpeed;
	if (cameraTimer > 1)
	{
		cameraTimer = 0;
		currentKeyFrame = (currentKeyFrame + 1) % cameraKeyFrameCount_Planet;
		std::cout << currentKeyFrame << "\n";
	}
	if (cameraTimer < 0)
	{
		cameraTimer = 1;
		currentKeyFrame--;
		if (currentKeyFrame < 0) currentKeyFrame = cameraKeyFrameCount_Planet - 1;
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

void Renderer::UpdateNodes(float dt)
{
	planetSurfaceRoot->Update(dt);
	spaceRoot->SetTransform(spaceRoot->GetTransform() * Matrix4::Rotation(-dt * 1.0f, Vector3(0, 1, 0)));
	planet->SetTransform(planet->GetTransform() * Matrix4::Rotation(-dt * 2.0f, Vector3(0, 0, 1)));
	spaceRoot->Update(dt);
}

float Renderer::naive_lerp(float a, float b, float t)
{
	return a + t * (b - a);
}
