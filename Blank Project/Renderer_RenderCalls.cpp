#include "Renderer.h"

void Renderer::RenderThings()
{
	switch (GetSceneType())
	{
	case(0):
		DrawPlanetSkybox();
		RenderTerrain();
		BuildNodeLists(planetSurfaceRoot);
		break;
	case(1):
		BuildNodeLists(spaceRoot);
		break;
	}
	//SortNodeLists();
	//DrawNodes();
	//ClearNodeLists();
}

void Renderer::DrawPlanetSkybox()
{
	glDepthMask(GL_FALSE);
	BindShader(skybox_Planet_Shader);
	UpdateShaderMatrices();
	skyBox->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::RenderTerrain()
{
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	SetShaderLight(*globalSceneLight);

	glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, groundBumpMap);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

}