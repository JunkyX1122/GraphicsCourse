#pragma once
#include "../NCLGL/OGLRenderer.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 int GetSceneType()
	 {
		 return renderSceneType;
	 }
	 void SetSceneType(int value)
	 {
		 renderSceneType = value;
	 }
protected:
	int renderSceneType;
	Mesh*	triangle;
	Shader* basicShader;
};
