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

	Shader* modelShader;
	Mesh* rockModel1;
	GLuint rockTexture1;
	GLuint rockBump1;

	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* combineShader;

	GLuint shadowTex;
	GLuint shadowFBO;
	Shader* shadowSceneShader;
	Shader* shadowShader;
	bool SetUpShadows();
	void DrawShadows();

	HeightMap* heightMap;
	GLuint seaBedTexture;
	GLuint seaBedBumpMap;
	GLuint groundTexture;
	GLuint groundBumpMap;
	GLuint highGroundTexture;
	GLuint highGroundBumpMap;
	void DrawTerrain();

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
	void DrawPointLights();

	Light* globalSceneLight;
	GLuint skyBox_Planet;
	GLuint skyBox_Space;
	Shader* skybox_Planet_Shader;
	Mesh* skyBox;
	void DrawSkybox();

	Mesh*	triangle;
	Shader* basicShader;
	GLuint basicTexture;
	GLuint basicBump;
};
