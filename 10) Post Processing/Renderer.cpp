#include "Renderer.h"

const int POST_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	camera = new Camera(-25.0f, 225.0f, Vector3(-150.0f, 1000.0f, -150.0f));
	quad = Mesh::GenerateQuad();

	heightMap = new HeightMap(TEXTUREDIR"white.png", 32.0f, 0);
	heightTexture = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	sceneShader = new Shader("texturedVertex.glsl", "texturedFragment.glsl");
	processShader = new Shader("texturedVertex.glsl", "processGetBrightFragment.glsl");
	processShader2 = new Shader("texturedVertex.glsl", "processFragment.glsl");
	processShader3 = new Shader("texturedVertex.glsl", "processPixelizeFragment.glsl");

	if (!processShader->LoadSuccess() || !sceneShader->LoadSuccess() || !heightTexture) return;

	SetTextureRepeating(heightTexture, true);

	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 3; i++)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	init = true;
	testValue = 0.0f;
	testValueRange = 301;
}

Renderer::~Renderer(void)
{
	delete sceneShader;
	delete processShader;
	delete heightMap;
	delete quad;
	delete camera;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::UpdateScene(float dt)
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
	{
		testValue -= 0.1;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
	{
		testValue += 0.1;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
	{
		testValueRange -= 1;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
	{
		testValueRange += 1;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
	{
		std::cout << "Bloom Intensity:" << testValue << "\n";
	}
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{
	DrawScene();
	DrawPostProcess();
	PresentScene();
}

void Renderer::DrawScene()
{

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	BindShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightTexture);
	heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < 1; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();
	}

	//=========================================================

	

	/*
	BindShader(processShader2);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0); 
	glUniform1i(glGetUniformLocation(processShader2->GetProgram(), "sceneTex"), 0);
	glUniform1i(glGetUniformLocation(processShader2->GetProgram(), "bloomRange"), testValueRange);
	glUniform1f(glGetUniformLocation(processShader2->GetProgram(), "bloomIntensity"), testValue);
	for (int i = 0; i < POST_PASSES * 2; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1 + ((i+1) % 2)], 0);
		glUniform1i(glGetUniformLocation(processShader2->GetProgram(), "isVertical"), i % 2);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1 + (i % 2)]);
		quad->Draw();
	}
	*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(processShader3);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(processShader3->GetProgram(), "sceneTexBase"), 0);
	glUniform2fv(glGetUniformLocation(processShader3->GetProgram(), "screenSize"), 1, (float*)&Vector2(1280, 720));
	if (testValue < 1)
	{
		testValue = 1;
	}
	glUniform1f(glGetUniformLocation(processShader3->GetProgram(), "pixelSize"), testValue);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
	//glUniform1i(glGetUniformLocation(processShader3->GetProgram(), "sceneTex"), 1);
	quad->Draw();
}