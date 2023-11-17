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
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_SHIFT))
	{
		cameraAutoMoveType = (cameraAutoMoveType + 1) % 2;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))
	{
		cameraFOV += 0.2f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS))
	{
		cameraFOV -= 0.2f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_0))
	{
		cameraFOV = 90.0f;
		camera->SetRotationSetter(Vector3(0.0f,0.0f,0.0f));
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
	{
		cameraAnimateSpeed += 0.001f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
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

void Renderer::UpdateCameraMovementPlanet(float dt)
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

void Renderer::UpdateCameraMovementSpace(float dt)
{
	cameraOrbitTimer += dt * -5.0f;
	Vector3 planetTransform = (spaceRoot->GetTransform() * Matrix4::Translation(Vector3(50000.0f,0.0f, 1500.0f))).GetPositionVector();
	Vector3 origin[2] = {Vector3(planetTransform.x, planetTransform.y, planetTransform.z), Vector3(0,0,0)};
	float distance[2] = { 2000.0f, 35000.0f };
	float angle = cameraOrbitTimer;
	float angleRad = angle * (PI / 180);
	float anglePitch = 20.0f * (PI / 180);
	float x = origin[cameraAutoMoveType].x + distance[cameraAutoMoveType] * cos(angleRad) * cos(anglePitch);
	float y = origin[cameraAutoMoveType].y + distance[cameraAutoMoveType] * sin(anglePitch);
	float z = origin[cameraAutoMoveType].z + distance[cameraAutoMoveType] * sin(angleRad) * cos(anglePitch);
	camera->SetPositionSetter
	(
		Vector3 (x, y, z )
	);
	camera->SetRotationSetter(Vector3(-20.0f, -angle + 90.0f, 0.0f));
}

void Renderer::UpdateNodes(float dt)
{
	planetSurfaceRoot->Update(dt);


	spaceRoot->		SetTransform(spaceRoot->		GetTransform() * Matrix4::Rotation(-dt * 1.0f, Vector3(0, 1, 0)));
	planet->		SetTransform(planet->			GetTransform() * Matrix4::Rotation(-dt * 2.0f, Vector3(0, 0, 1))); 
	asteroidParent->SetTransform(asteroidParent->	GetTransform() * Matrix4::Rotation(-dt * 8.0f, Vector3(0, 1, 0)));
	sun->			SetTransform(sun->				GetTransform() * Matrix4::Rotation(-dt * 4.0f, Vector3(0, 0, 1)));

	for (int i = 0; i < asteroidParent->GetChildCount(); i++)
	{
		SceneNode* s = asteroidParent->GetChild(i);
		s->SetTransform
		(
			s->GetTransform()
			* Matrix4::Rotation(dt * 69.0f * s->GetRotationSpeed().x, Vector3(1, 0, 0))
			* Matrix4::Rotation(dt * 69.0f * s->GetRotationSpeed().y, Vector3(0, 1, 0))
			* Matrix4::Rotation(dt * 69.0f * s->GetRotationSpeed().z, Vector3(0, 0, 1))
		);
	}
	spaceRoot->Update(dt);
}

float Renderer::naive_lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

