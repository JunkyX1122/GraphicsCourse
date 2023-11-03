#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include <algorithm>

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

	SceneNode* planetSurfaceRoot;
	SceneNode* spaceRoot;
	Frustum frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	bool ManageSceneNodes();
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNodes(SceneNode* n);


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
