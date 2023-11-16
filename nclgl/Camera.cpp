#include "Camera.h"
#include "Window.h"
#include <algorithm>
using namespace std;
void Camera::UpdateCamera(float dt)
{
	pitch	-= (Window::GetMouse()->GetRelativePosition().y);
	yaw		-= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch,  90.0f);
	pitch = std::max(pitch, -90.0f);
	
	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
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
		roll += 0.5f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E))
	{
		roll -= 0.5f;
	}
	position = position * 0.95f + positionSetter * 0.05f;

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
	{
		cout <<
			"X:		" << position.x << endl <<
			"Y:		" << position.y << endl <<
			"Z:		" << position.z << endl <<
			"PITCH:	" << pitch << endl <<
			"YAW:	" << yaw << endl <<
			"ROLL:	" << roll << endl
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