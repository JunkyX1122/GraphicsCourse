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
	GLuint seaBedTexture;
	GLuint seaBedBumpMap;
	GLuint groundTexture;
	GLuint groundBumpMap;
	GLuint highGroundTexture;
	GLuint highGroundBumpMap;
	void RenderTerrain();

	GLuint waterTex;
	float waterRotate;
	float waterCycle;
	Shader* reflectShader;
	Mesh* waterQuad;
	void DrawWater();

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Mesh* quad;

	Light* globalSceneLight;
	GLuint skyBox_Planet;
	Shader* skybox_Planet_Shader;
	Mesh* skyBox;
	void DrawPlanetSkybox();

	Mesh*	triangle;
	Shader* basicShader;
};
