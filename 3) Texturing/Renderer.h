#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public  OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);
	virtual void RenderScene();

	void UpdateTextureMatrix(float value, float timerValue);
	void ToggleRepeating();
	void ToggleFiltering();

protected:
	Mesh* triangle;
	Shader* shader;
	GLuint texture;
	bool filtering;
	bool repeating;
};