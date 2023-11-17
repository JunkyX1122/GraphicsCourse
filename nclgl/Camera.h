#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera {
public:
	Camera(void)
	{
		cameraFreeMove = true;
		yaw = 0.0f;
		pitch = 0.0f;
		roll = 0.0f;
	}
	Camera(float pitch, float yaw, Vector3 position)
	{
		cameraFreeMove = true;
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		this->positionSetter = position;
		roll = 0.0f;
	}
	~Camera(void) {};
	
	void UpdateCamera(float dt = 1.0f);
	
	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	Vector3 GetPositionSetter() const { return positionSetter; }
	void SetPositionSetter(Vector3 val) { positionSetter = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float val) { yaw = val; }

	float GetPitch() const { return pitch; }
	void SetPitch(float val) { pitch = val; }

	float GetRoll() const { return roll; }
	void SetRoll(float val) { roll = val; }

	Vector3 GetRotation() const { return Vector3(pitch, yaw, roll); }
	void SetRotation(Vector3 val) { pitch = val.x; yaw = val.y; roll = val.z; }

	Vector3 GetRotationSetter() const { return rotationsSetter; }
	void SetRotationSetter(Vector3 val) { rotationsSetter = val; }

	void LockFreeMovement() { cameraFreeMove = false; }
	void UnlockFreeMovement() { cameraFreeMove = true; }
	bool GetFreeMove() { return cameraFreeMove; }

protected:
	bool cameraFreeMove;
	float pitch;
	float yaw;
	float roll;
	Vector3 rotationsSetter;
	Vector3 position;
	Vector3 positionSetter;
};