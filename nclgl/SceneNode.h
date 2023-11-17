#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

#define SCENENODETAG_NULL -1
#define SCENENODETAG_PLANET 0
#define SCENENODETAG_SUN 1


class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	SceneNode(Matrix4& matrix, Vector3 s, float f, Mesh* m, GLuint tex, GLuint b)
	{
		transform = matrix;
		modelScale = s;
		boundingRadius = f;
		mesh = m;
		texture = tex;
		bump = b;
		localRotationSpeed = Vector3(0, 0, 0);
	}

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	void SetWorldTransform(const Matrix4& matrix) { worldTransform = matrix; }
	const Matrix4& GetTransform()				const { return transform; }
	Matrix4 GetWorldTransform()					const { return worldTransform; }

	Vector4 GetColour() const { return color; }
	void SetColour(Vector4 c) { color = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	Vector3 GetRotationSpeed() const { return localRotationSpeed; }
	void SetRotationSpeed(Vector3 s) { localRotationSpeed = s; }

	void AddChild(SceneNode* s);

	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	GLuint GetTexture() const { return texture; }
	void SetTexture(GLuint tex) { texture = tex; }

	GLuint GetTexture2() const { return texture2; }
	void SetTexture2(GLuint tex) { texture2 = tex; }

	GLuint GetBump() const { return bump; }
	void SetBump(GLuint b) { bump = b; }

	int GetTag() { return tag; }
	void SetTag(int i) { tag = i; }

	int GetLightingInfluence() { return lightingInfluence; }
	void SetLightingInfluence(int b) { lightingInfluence = b; }



	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart()
	{
		return children.begin();
	}
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd()
	{
		return children.end();
	}

	int GetChildCount() { return children.size(); }
	SceneNode* GetChild(int i) { return children[i]; }
	
	
protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 color;
	std::vector<SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;
	GLuint texture2;
	GLuint bump;
	Vector3 localRotationSpeed;

	int tag = SCENENODETAG_NULL;
	int lightingInfluence = 1;
};
