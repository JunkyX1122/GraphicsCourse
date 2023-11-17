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
	 void TransitionCall() { transitionFlag = 2; transitionTimer = 1.0f; }
	 bool IsTransitioning() { return transitionFlag != 0; }
	 Camera* GetCamera() { return camera; }
	
	 float manualMove = 0.0f;
protected:
	Vector4 emptyColour;
	int renderSceneType;
	int transitionFlag;
	float transitionTimer;
	void Transition(float dt);
	Vector3 storedCamPosition[2];
	Vector3 storedCamRotation[2];
	

	Camera* camera;
	int cameraAutoMoveType;
	float cameraTimer;
	float cameraOrbitTimer;
	float introTimer;
	bool introFlag;
	float timePerBar;
	vector<Vector3> cameraPositions_Planet;
	vector<Vector3> cameraRotations_Planet;
	vector<float> cameraKeyTimes_Planet;
	int cameraKeyFrameCount_Planet;
	int currentKeyFrame;
	float cameraAnimateSpeed;
	float cameraFOV;
	bool SetUpCamera();
	void AddCameraKeyFrame(Vector3 pos, Vector3 rot, float timeToReach);
	void UpdateCameraControls();
	void UpdateCameraMovementPlanet(float dt);
	void UpdateCameraMovementSpace(float dt);
	float naive_lerp(float a, float b, float t);

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
	void UpdateNodes(float dt);

	Shader* modelShader;
	Mesh* rockModel1;
	GLuint rockTexture1;
	GLuint rockBump1;
	bool SetUpRocks();

	Mesh* asteroidModel1;
	GLuint asteroidTexture1;
	GLuint asteroidBump1;
	SceneNode* asteroidParent;
	bool SetUpAsteroids();

	Mesh* crystalModel1;
	GLuint crystalTexture1;
	GLuint crystalBump1;
	bool SetUpCrystals();

	Mesh* planetModel;
	GLuint planetTexture;
	GLuint planetCloudTexture;
	GLuint planetBump;
	Shader* planetShader;
	SceneNode* planet;
	float planetCycle;
	bool SetUpMainPlanet();

	GLuint sunTexture;
	SceneNode* sun;
	bool SetUpSun();

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
	GLuint bufferColourTex[4];
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;
	void ManagePostProcess();
	void DrawPostProcess();

	std::vector<Vector3> pointLightPositions;
	std::vector<Vector4> pointLightColours;
	std::vector<float> pointLightStarts;
	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Mesh* sphere;
	Mesh* quad;
	Light* pointLights;
	float pointLightCycle;
	bool SetUpPointLights();
	void DrawPointLights();
	void UpdatePointLights(float dt);

	Light* globalSceneLight;
	GLuint skyBox_Planet;
	GLuint skyBox_Space;
	Shader* skybox_Planet_Shader;
	Mesh* skyBox;
	bool SetUpSkybox();
	void DrawSkybox();

	GLuint processFBO;
	GLuint processFinalFBO;
	Shader* processShaderGetBright;
	Shader* processShaderBlur;
	Shader* processShaderBloom;
	Shader* processShaderPixelize;
	float pixelSize;
	Vector2 screenSize;
	Vector4 colourCorrection;

	Mesh*	triangle;
	Shader* basicShader;
	GLuint basicTexture;
	GLuint basicBump;

	bool easterEgg = false;
	GLuint easterEggTex;
};
