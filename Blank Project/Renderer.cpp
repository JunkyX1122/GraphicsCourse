#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	renderSceneType = 0;
	quad = Mesh::GenerateQuad();
	skyBox = Mesh::GenerateQuad();
	waterQuad = Mesh::GenerateQuad();
	//========================================================================
	heightMap = new HeightMap(TEXTUREDIR"noise.png", 16.0f);
	if (!heightMap) return;
	//========================================================================
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(waterTex, true);
	if (!waterTex) return;

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	if (!reflectShader->LoadSuccess()) return;

	waterRotate = 0.0f;
	waterCycle = 0.0f;
	//========================================================================
	seaBedTexture = SOIL_load_OGL_texture(TEXTUREDIR"white.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	seaBedBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"white.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	groundTexture = SOIL_load_OGL_texture(TEXTUREDIR"red.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"white.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	highGroundTexture = SOIL_load_OGL_texture(TEXTUREDIR"black.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	highGroundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"white.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(seaBedTexture, true);
	SetTextureRepeating(seaBedBumpMap, true);

	SetTextureRepeating(groundTexture, true);
	SetTextureRepeating(groundBumpMap, true);

	SetTextureRepeating(highGroundTexture, true);
	SetTextureRepeating(highGroundBumpMap, true);

	if (!seaBedTexture) return;
	if (!seaBedBumpMap) return;

	if (!groundTexture) return;
	if (!groundBumpMap) return;

	if (!highGroundTexture) return;
	if (!highGroundBumpMap) return;
	//========================================================================
	skyBox_Planet =
		SOIL_load_OGL_cubemap(
			TEXTUREDIR"rusted_west_new.JPG",
			TEXTUREDIR"rusted_east_new.JPG",
			TEXTUREDIR"rusted_up_new.JPG",
			TEXTUREDIR"rusted_down_new.JPG",
			TEXTUREDIR"rusted_south_new.JPG",
			TEXTUREDIR"rusted_north_new.JPG",
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			0);
	if (!skyBox_Planet) return;

	skybox_Planet_Shader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	if (!skybox_Planet_Shader->LoadSuccess()) return;
	//========================================================================
	Vector3 heightMapSize = heightMap->GetHeightMapSize();
	camera = new Camera(-45.0f, 0.0f, heightMapSize * Vector3(0.5f, 1.0f, 0.5f));
	globalSceneLight = new Light(heightMapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x);
	//========================================================================
	sceneShader = new Shader("terrainAdvanceVertex.glsl", "terrainAdvanceFragment.glsl");
	pointLightShader = new Shader("pointLightVertex.glsl", "pointLightFragment.glsl");
	combineShader = new Shader("combineVertex.glsl", "combineFragment.glsl");

	basicShader = new Shader("sceneVertex.glsl", "sceneFragment.glsl");

	if (!sceneShader->LoadSuccess()) return;
	if (!pointLightShader->LoadSuccess()) return;
	if (!combineShader->LoadSuccess()) return;
	//========================================================================
	planetSurfaceRoot = new SceneNode();
	spaceRoot = new SceneNode();

	if (!ManageSceneNodes()) return;
	if (!CreateBuffers()) return;
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

	glDeleteTextures(1, &bufferColourTex);
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
	
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 30000.0f, (float)width / (float)height, 45.0f);
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.25f;
	planetSurfaceRoot->Update(dt);
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
	
}



