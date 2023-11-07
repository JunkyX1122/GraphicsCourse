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
	glUniform1f(glGetUniformLocation(sceneShader->GetProgram(), "terrainLowBoundry"), (heightMap->GetHeightMapSize().y) / 3);
	glUniform1f(glGetUniformLocation(sceneShader->GetProgram(), "terrainHighBoundry"), (heightMap->GetHeightMapSize().y) / 3 * 2);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex_low"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, seaBedTexture);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex_low"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, seaBedBumpMap);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex_mid"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex_mid"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, groundBumpMap);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex_high"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, highGroundTexture);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex_high"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, highGroundBumpMap);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

}