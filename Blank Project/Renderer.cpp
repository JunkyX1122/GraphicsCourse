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
	cameraTimer = 0.0f;
	cameraAutoMoveType = 0;
	camera = new Camera(-45.0f, 0.0f, heightMapSize * Vector3(0.5f, 1.0f, 0.5f));
	camera->LockFreeMovement();
	globalSceneLight = new Light(heightMapSize * Vector3(0.5f, 1.0f, 0.5f), Vector4(1, 1, 1, 1), heightMapSize.x);
	SetUpCameraKeyFrames();
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

	basicShader = new Shader("sceneVertex.glsl", "sceneFragment.glsl");
	modelShader = new Shader("bumpVertex.glsl", "bumpFragment.glsl");
	planetShader = new Shader("planetVertex.glsl", "planetFragment.glsl");
	
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

	delete modelShader;
	delete rockModel1;
	glDeleteTextures(1, &rockTexture1);
	glDeleteTextures(1, &rockBump1);

	delete crystalModel1;
	glDeleteTextures(1, &crystalTexture1);
	glDeleteTextures(1, &crystalBump1);

	delete planetModel;
	glDeleteTextures(1, &planetTexture);
	glDeleteTextures(1, &planetCloudTexture);
	glDeleteTextures(1, &planetBump);
	delete planetShader;

	delete sceneShader;
	delete pointLightShader;
	delete combineShader;

	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(1, &shadowTex);
	delete shadowSceneShader;
	delete shadowShader;

	delete heightMap;
	glDeleteTextures(1, &seaBedTexture);
	glDeleteTextures(1, &seaBedBumpMap);
	glDeleteTextures(1, &groundTexture);
	glDeleteTextures(1, &groundBumpMap);
	glDeleteTextures(1, &highGroundTexture);
	glDeleteTextures(1, &highGroundBumpMap);

	glDeleteTextures(1, &waterTex);
	delete reflectShader;
	delete waterQuad;

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteTextures(4, bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);

	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	delete sphere;
	delete quad;
	delete[] pointLights;

	delete globalSceneLight;
	glDeleteTextures(1, &skyBox_Planet);
	glDeleteTextures(1, &skyBox_Space);
	delete skybox_Planet_Shader;
	delete skyBox;

	glDeleteFramebuffers(1, &processFBO);
	delete processShaderGetBright;
	delete processShaderBlur;
	delete processShaderBloom;

	delete camera;
	delete heightMap;
	delete quad;
	delete globalSceneLight;

	delete sceneShader;
	delete combineShader;
	delete pointLightShader;

	delete triangle;
	delete basicShader;
	glDeleteTextures(1, &basicTexture);
	glDeleteTextures(1, &basicBump);
}

void Renderer::UpdateScene(float dt) 
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS))
	{
		cameraAnimateSpeed += 0.001f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS))
	{
		cameraAnimateSpeed -= 0.001f;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT))
	{
		cameraAnimateSpeed = 1.0f/8.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT))
	{
		cameraAnimateSpeed = -1.0f/8.0f;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M))
	{
		if (camera->GetFreeMove()) 
		{
			camera->LockFreeMovement();
		}
		else
		{
			camera->UnlockFreeMovement();
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_N))
	{
		std::cout << "Current Cam Keyframe: " << currentKeyFrame << "\n";
		camera->SetPositionSetter(cameraPositions_Planet[currentKeyFrame]);
		camera->SetRotationSetter(cameraRotations_Planet[currentKeyFrame]);
		currentKeyFrame = (currentKeyFrame + 1) % cameraKeyFrameCount_Planet;
	}
	if(!camera->GetFreeMove()) UpdateCameraMovement(dt);
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



