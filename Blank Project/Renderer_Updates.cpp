#include "Renderer.h"

void Renderer::UpdateSceneLight(float dt)
{
	Vector3 heightMapCentre = heightMapSize * Vector3(0.5f, 0, 0.5f);
	float camDist = heightMapSize.y * 5.0f;
	switch (GetSceneType())
	{
	case(0):
	{
		if (currentKeyFrame >= 14 && currentKeyFrame < 16)
		{
			globalLightTimer += dt / (32.0f * timePerBar);
		}
		else
		{
			globalLightTimer = 0;
		}
		float camAngPitch = PI / 2 + (PI/2) * sin((4*PI) * globalLightTimer);
		float dayScale = sin(camAngPitch);
		float camAngYaw = 2*PI * globalLightTimer;
		Vector3 camPos = Vector3(
			heightMapCentre.x + camDist * cos(camAngYaw) * cos(camAngPitch),
			heightMapCentre.y + camDist * sin(camAngPitch),
			heightMapCentre.z + camDist * sin(camAngYaw) * cos(camAngPitch));
		globalSceneLight->SetPosition(camPos);
		globalSceneLight->SetRadius(80000.0f);
		globalSceneLight->SetColour(Vector4(0.25f + 0.75f * dayScale, 0.1f + 0.9f * dayScale, 1.0f * dayScale, 1.0f * dayScale));
		break;
	}
	case(1):
	{
		globalSceneLight->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		globalSceneLight->SetRadius(80000.0f);
		globalSceneLight->SetColour(Vector4(1.0f, 0.85f, 0.45f, 1.0f));
		break;
	}
	}
}

void Renderer::UpdatePointLights(float dt)
{
	pointLightCycle += dt * 0.25f;
	for (int i = 0; i < pointLightPositions.size(); i++)
	{
		Light& l = pointLights[i];
		float rad = (3000.0f + 500.0f * sin(2 * PI * pointLightCycle + pointLightStarts[i])) * pointLightScales[i];
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
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_9))
	{
		togglePostProcess = !togglePostProcess;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
	{
		cameraAnimateSpeed += 0.001f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
	{
		cameraAnimateSpeed = cameraAnimateSpeed < 0 ? 0 : cameraAnimateSpeed - 0.001f;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT))
	{
		cameraAnimateSpeed = 1.0f;
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
	int nextKeyFrame = (currentKeyFrame + 1) % cameraKeyFrameCount_Planet;
	cameraTimer += dt * cameraAnimateSpeed / cameraKeyTimes_Planet[nextKeyFrame];
	if (cameraTimer > 1)
	{
		if (nextKeyFrame == cameraKeyFrameCount_Planet-1) loopCount++;
		if (loopCount >= LOOP_PLANET_SCENE && !IsTransitioning()) TransitionCall();
		cameraTimer = 0;
		currentKeyFrame = nextKeyFrame;
	}
	Vector3 pos1 = cameraPositions_Planet[currentKeyFrame];
	Vector3 pos2 = cameraPositions_Planet[nextKeyFrame];
	Vector3 posTrans = Vector3(
		naive_lerp(pos1.x, pos2.x, cameraTimer),
		naive_lerp(pos1.y, pos2.y, cameraTimer),
		naive_lerp(pos1.z, pos2.z, cameraTimer)
	);
	camera->SetPositionSetter(posTrans);

	Vector3 rot1 = cameraRotations_Planet[currentKeyFrame];
	Vector3 rot2 = cameraRotations_Planet[nextKeyFrame];
	Vector3 rotTrans = Vector3(
		naive_lerp(rot1.x, rot2.x, cameraTimer),
		naive_lerp(rot1.y, rot2.y, cameraTimer),
		naive_lerp(rot1.z, rot2.z, cameraTimer)
	);
	camera->SetRotationSetter(rotTrans);
}

void Renderer::UpdateCameraMovementSpace(float dt)
{
	if (cameraAutoMoveType == 0 && -cameraOrbitTimer > 360.0f && !camera->GetFreeMove())
	{
		cameraAutoMoveType++;
	}
	cameraOrbitTransition += dt / (16.0f * timePerBar) * (-1 + 2 *cameraAutoMoveType);
	if (cameraOrbitTransition > 1.f) cameraOrbitTransition = 1.0f;
	if (cameraOrbitTransition < 0.f) cameraOrbitTransition = 0.0f;

	cameraOrbitTimer += dt * (360.0f/(32.0f * timePerBar)) * -1;
	Vector3 planetTransform = (spaceRoot->GetTransform() * Matrix4::Translation(Vector3(50000.0f,0.0f, 1500.0f))).GetPositionVector();
	Vector3 origin[2] = {Vector3(planetTransform.x, planetTransform.y, planetTransform.z), Vector3(0,0,0)};
	float distance[2] = { 2000.0f, 35000.0f };
	float angle = cameraOrbitTimer;
	float angleRad = angle * (PI / 180);
	float anglePitch = 20.0f * (PI / 180);
	float x1 = origin[0].x + distance[0] * cos(angleRad) * cos(anglePitch);
	float y1 = origin[0].y + distance[0] * sin(anglePitch);
	float z1 = origin[0].z + distance[0] * sin(angleRad) * cos(anglePitch);

	float x2 = origin[1].x + distance[1] * cos(angleRad) * cos(anglePitch);
	float y2 = origin[1].y + distance[1] * sin(anglePitch);
	float z2 = origin[1].z + distance[1] * sin(angleRad) * cos(anglePitch);
	camera->SetPositionSetter
	(
		Vector3 (
			naive_lerp(x1, x2, cameraOrbitTransition), 
			naive_lerp(y1, y2, cameraOrbitTransition), 
			naive_lerp(z1, z2, cameraOrbitTransition))
	);
	camera->SetRotationSetter(Vector3(-20.0f, -angle + 90.0f, 0.0f));
}

void Renderer::UpdateNodes(float dt)
{
	switch (GetSceneType())
	{
	case(0):
		planetSurfaceRoot->Update(dt);
		break;

	case(1):
		if (
			Window::GetKeyboard()->KeyDown(KEYBOARD_1) && 
			Window::GetKeyboard()->KeyDown(KEYBOARD_9) && 
			Window::GetKeyboard()->KeyDown(KEYBOARD_4) )
		{
			easterEgg = true;
		}
		else
		{
			easterEgg = false;
		}
		spaceRoot->SetTransform(spaceRoot->GetTransform() * Matrix4::Rotation(-dt * 1.0f, Vector3(0, 1, 0)));
		planet->SetTransform(planet->GetTransform() * Matrix4::Rotation(-dt * 2.0f, Vector3(0, 0, 1)));
		asteroidParent->SetTransform(asteroidParent->GetTransform() * Matrix4::Rotation(-dt * 8.0f, Vector3(0, 1, 0)));
		
		if (easterEgg)
		{
			sun->SetBoundingRadius(5000.0f * 3);
			sun->SetModelScale(Vector3(5000.0f, 5000.0f, 5000.0f) * 3);
		}
		else
		{
			sun->SetBoundingRadius(5000.0f);
			sun->SetModelScale(Vector3(5000.0f, 5000.0f, 5000.0f));
		}
		float spd = easterEgg ? 64.0f * 4.0f : 4.0f;
		sun->SetTransform(sun->GetTransform() * Matrix4::Rotation(-dt * spd, Vector3(0, 0, 1)));
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
		break;
	}
}

void Renderer::HandleIntro(float dt)
{
	float barsToIntro = 4.0f;
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_SPACE) && introTimer > 0.0f)
	{
		introTimer = 1.0f;
	}
	if (introTimer < 1)
	{
		introTimer += dt / (barsToIntro * timePerBar);
		colourCorrection = Vector4(introTimer, introTimer, introTimer, 1.0f);
	}
	else
	{
		introFlag = false;
		introTimer = 1.0f;
		colourCorrection = Vector4(introTimer, introTimer, introTimer, 1.0f);
		cameraAnimateSpeed = 1.0f;
	}
}

void Renderer::UpdateAnimation(float dt)
{
	animationTimer -= dt * 0.5f;
	while (animationTimer < 0.0f)
	{
		currentAnimatedFrame = (currentAnimatedFrame + 1) % anim->GetFrameCount();
		animationTimer += 1.0f / anim->GetFrameRate();
	}
}

float Renderer::naive_lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

