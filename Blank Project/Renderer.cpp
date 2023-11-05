#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	renderSceneType = 0;
	quad = Mesh::GenerateQuad();
	skyBox = Mesh::GenerateQuad();
	//========================================================================
	heightMap = new HeightMap(TEXTUREDIR"terrain_1.png", 2048.0f);
	groundTexture = SOIL_load_OGL_texture(TEXTUREDIR"terrain_1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"white.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(groundTexture, true);
	SetTextureRepeating(groundBumpMap, true);

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

	if (!heightMap) return;
	if (!groundTexture) return;
	if (!groundBumpMap) return;
	if (!skyBox_Planet) return;
	Vector3 heightMapSize = heightMap->GetHeightMapSize();
	//========================================================================
	camera = new Camera(-45.0f, 0.0f, heightMapSize * Vector3(0.5f, 5.0f, 0.5f));
	
	globalSceneLight = new Light(heightMapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x);
	//========================================================================
	sceneShader = new Shader("bumpVertex.glsl", "bufferFragment.glsl");
	pointLightShader = new Shader("pointLightVertex.glsl", "pointLightFragment.glsl");
	combineShader = new Shader("combineVertex.glsl", "combineFragment.glsl");

	skybox_Planet_Shader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	//skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	if (!sceneShader->LoadSuccess()) return;
	if (!pointLightShader->LoadSuccess()) return;
	if (!combineShader->LoadSuccess()) return;
	if (!skybox_Planet_Shader->LoadSuccess()) return;
	//========================================================================
	if (!ManageSceneNodes()) return;

	//========================================================================
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

	delete triangle;
	delete basicShader;
	
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
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



