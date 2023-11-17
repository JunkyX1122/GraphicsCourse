#include "Camera.h"
#include "Window.h"
#include <algorithm>
using namespace std;
void Camera::UpdateCamera(float dt)
{
	if (GetFreeMove())
	{
		rotationsSetter.x -= (Window::GetMouse()->GetRelativePosition().y);
		rotationsSetter.y -= (Window::GetMouse()->GetRelativePosition().x);
	}
	rotationsSetter.x = std::min(rotationsSetter.x,  90.0f);
	rotationsSetter.x = std::max(rotationsSetter.x, -90.0f);
	
	if (yaw < 0)
	{
		//yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		//yaw -= 360.0f;
	}



	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	float speedMultiplier = 7.5;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL))
	{
		speedMultiplier *= 9.0f;
	}

	float speed = 120.0f * dt * speedMultiplier;
	if (GetFreeMove())
	{
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
		{
			positionSetter += forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
		{
			positionSetter -= forward * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
		{
			positionSetter -= right * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
		{
			positionSetter += right * speed;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		{
			positionSetter.y -= speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		{
			positionSetter.y += speed;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q))
		{
			rotationsSetter.z += 0.5f;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_E))
		{
			rotationsSetter.z -= 0.5f;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_E))
		{
			//rotationsSetter.z -= 0.5f;
		}
	}
	float dampenCam = 0.01f;
	float dampenRot = 0.01f;
	if (GetFreeMove()) 
	{
		dampenCam = 0.75f;
		dampenRot = 0.75f;
	}
	position = position * (1.0f - dampenCam) + positionSetter * dampenCam;
	pitch = pitch * (1.0f - dampenRot) + rotationsSetter.x * dampenRot;
	yaw = yaw * (1.0f - dampenRot) + rotationsSetter.y * dampenRot;
	roll = roll * (1.0f - dampenRot) + rotationsSetter.z * dampenRot;

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
	{
		cout 
			<< position.x << "," 
			<< position.y << "," 
			<< position.z << endl  
			<< pitch << "," 
			<< yaw << "," 
			<< roll << endl
			;
	}
}

Matrix4 Camera::BuildViewMatrix()
{
	return	
		Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);
}