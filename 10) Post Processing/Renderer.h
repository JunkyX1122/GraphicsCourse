#pragma once
#include "../nclgl/OGLRenderer.h "
#include "../nclgl/HeightMap.h "
#include "../nclgl/Camera.h "

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	Shader* sceneShader;
	Shader* getBrightShader;
	Shader* processShader;
	Shader* combineShader;

	Camera* camera;

	Mesh* quad;
	HeightMap* heightMap;
	GLuint heightTexture;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[3];
	GLuint bufferDepthTex;
};