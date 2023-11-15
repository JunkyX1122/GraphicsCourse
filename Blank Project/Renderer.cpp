#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	renderSceneType = 0;
	quad = Mesh::GenerateQuad();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	skyBox = Mesh::GenerateQuad();
	waterQuad = Mesh::GenerateQuad();
	
	
	if (!sphere) return;

	//========================================================================
	
	if (!SetUpTerrain()) return;
	
	if (!SetUpWater()) return;

	if (!SetUpSkybox()) return;
	//========================================================================

	camera = new Camera(-45.0f, 0.0f, heightMapSize * Vector3(0.5f, 1.0f, 0.5f));
	globalSceneLight = new Light(heightMapSize * Vector3(0.5f, 1.0f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x);
	//========================================================================
	
	planetSurfaceRoot = new SceneNode();
	spaceRoot = new SceneNode();
	spaceRoot->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));

	if (!ManagePlanetSurfaceSceneNodes()) return;
	if (!CreateBuffers()) return;
	if (!ManageSpaceSceneNodes()) return;

	//========================================================================
	if (!SetUpPointLights()) return;
	//========================================================================


	sceneShader = new Shader("terrainAdvanceVertex.glsl", "terrainAdvanceFragment.glsl");
	pointLightShader = new Shader("pointLightVertex.glsl", "pointLightFragment.glsl");
	combineShader = new Shader("combineVertex.glsl", "combineFragment.glsl");

	basicShader = new Shader("sceneVertex.glsl", "sceneFragment.glsl");
	modelShader = new Shader("bumpVertex.glsl", "bumpFragment.glsl");
	planetShader = new Shader("planetVertex.glsl", "planetFragment.glsl");
	if (!sceneShader->LoadSuccess()) return;
	if (!pointLightShader->LoadSuccess()) return;
	if (!combineShader->LoadSuccess()) return;
	if (!basicShader->LoadSuccess()) return;
	if (!modelShader->LoadSuccess()) return;
	if (!planetShader->LoadSuccess()) return;

	//========================================================================
	
	//========================================================================
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	init = true;
}


//

Renderer::~Renderer(void)	
{
	delete camera;
	delete heightMap;
	delete quad;
	delete globalSceneLight;

	delete sceneShader;
	delete combineShader;
	delete pointLightShader;

	glDeleteTextures(3, bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

	delete skybox_Planet_Shader;
	delete skyBox;

	delete waterQuad;

	//delete triangle;
	delete basicShader;

	delete modelShader;
	delete planetShader;
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 90000.0f, (float)width / (float)height, 90.0f);
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;
	planetCycle += dt * 1.0f;
	
	UpdatePointLights(dt);


	planetSurfaceRoot->Update(dt);
	spaceRoot->SetTransform(spaceRoot->GetTransform() * Matrix4::Rotation(-dt * 1.0f, Vector3(0, 1, 0)));
	planet->SetTransform(planet->GetTransform() * Matrix4::Rotation(-dt * 2.0f, Vector3(0, 0, 1)));
	spaceRoot->Update(dt);
	
}

void Renderer::RenderScene()	
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	

	FillBuffers();
	DrawPointLights();
	CombineBuffers();
	ManagePostProcess();
	DrawPostProcess();
}



