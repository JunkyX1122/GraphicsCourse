#include "Renderer.h"
#include <cmath>
Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();

	matrixShader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");

	camera = new Camera();

	if (!matrixShader->LoadSuccess()) {
		return;
	}

	init = true;
	SwitchToOrthographic();
}

Renderer::~Renderer(void)
{
	delete triangle;
	delete matrixShader;
	delete camera;
}

void Renderer::SwitchToPerspective()
{
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, float(width) / float(height), fov);
}

void Renderer::SwitchToOrthographic()
{
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(matrixShader);

	glUniformMatrix4fv(glGetUniformLocation(matrixShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(matrixShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);

	for (int i = 0; i < 20; i++)
	{
		for (int o = 0; o < 5; o++)
		{
			double xDegrees = (10.0f+360.0f/5.0f*o) * i * PI / 180;

			Vector3 tempPos = position;
			tempPos.z += (i * 100.0f);
			tempPos.y += 20.0f * (sin(xDegrees)) * i;
			tempPos.x += 20.0f * (cos(xDegrees)) * i;

			modelMatrix =
				Matrix4::Translation(tempPos) *
				Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
				Matrix4::Scale(Vector3(scale, scale, scale));


			glUniformMatrix4fv(glGetUniformLocation(matrixShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
			triangle->Draw();
		}
	}
}

void Renderer::UpdateScene(float dt)
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}