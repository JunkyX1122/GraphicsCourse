#pragma once
#include "../NCLGL/OGLRenderer.h"
class Camera;
class HeightMap;
class Light;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 bool CreateBuffers();
	 void GenerateScreenTexture(GLuint& into, bool depth = false);

	 void UpdateScene(float msec)	override;

	 int GetSceneType() { return renderSceneType; };
	 void SetSceneType(int value) { renderSceneType = value; };
	 
	 void FillBuffers();
	 void RenderThings();
	 void DrawPointLights();
	 void CombineBuffers();
protected:
	int renderSceneType;
	Camera* camera;

	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* combineShader;

	HeightMap* heightMap;
	GLuint groundTexture;
	GLuint groundBumpMap;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Mesh* quad;

	Light* light;

	Mesh*	triangle;
	Shader* basicShader;
};
