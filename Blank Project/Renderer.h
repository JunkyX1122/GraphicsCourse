#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include <algorithm>
#define LIGHT_NUM 32
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

	 float manualMove = 0.0f;
protected:
	int renderSceneType;
	Camera* camera;

	SceneNode* planetSurfaceRoot;
	SceneNode* spaceRoot;
	Frustum frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	bool ManagePlanetSurfaceSceneNodes();
	bool ManageSpaceSceneNodes();
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNodes(SceneNode* n);

	Shader* modelShader;
	Mesh* rockModel1;
	GLuint rockTexture1;
	GLuint rockBump1;

	Mesh* planetModel;
	GLuint planetTexture;
	GLuint planetCloudTexture;
	GLuint planetBump;
	Shader* planetShader;
	float planetCycle;

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
	Vector3 heightMapSize;
	bool SetUpTerrain();
	void DrawTerrain();

	GLuint waterTex;
	float waterRotate;
	float waterCycle;
	Shader* reflectShader;
	Mesh* waterQuad;
	bool SetUpWater();
	void DrawWater();

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Mesh* sphere;
	Mesh* quad;
	Light* pointLights;
	bool SetUpPointLights();
	void DrawPointLights();

	Light* globalSceneLight;
	GLuint skyBox_Planet;
	GLuint skyBox_Space;
	Shader* skybox_Planet_Shader;
	Mesh* skyBox;
	bool SetUpSkybox();
	void DrawSkybox();

	Mesh*	triangle;
	Shader* basicShader;
	GLuint basicTexture;
	GLuint basicBump;


};
