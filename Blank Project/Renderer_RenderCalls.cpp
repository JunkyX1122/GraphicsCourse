#include "Renderer.h"

void Renderer::RenderThings()
{
	DrawSkybox();
	switch (GetSceneType())
	{
	case(0):
		globalSceneLight->SetPosition(heightMapSize * Vector3(0.5f, 3.0f, 0.5f) + Vector3(-5000.0f,0,0));
		globalSceneLight->SetRadius(80000.0f);
		globalSceneLight->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		DrawTerrain();
		BuildNodeLists(planetSurfaceRoot);
		SortNodeLists();
		DrawNodes();
		DrawWater();
		break;
	case(1):
		globalSceneLight->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		globalSceneLight->SetRadius(80000.0f);
		globalSceneLight->SetColour(Vector4(1.0f, 0.85f, 0.45f, 1.0f));
		BuildNodeLists(spaceRoot);
		SortNodeLists();
		DrawNodes();
		break;
	}
	
	ClearNodeLists();
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	BindShader(skybox_Planet_Shader);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_CUBE_MAP, GetSceneType() == 0 ? skyBox_Planet : skyBox_Space);

	
	UpdateShaderMatrices();
	skyBox->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawTerrain()
{
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	SetShaderLight(*globalSceneLight);

	glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1f(glGetUniformLocation(sceneShader->GetProgram(), "terrainLowBoundry"), (heightMap->GetHeightMapSize().y) / 8 * 2);
	glUniform1f(glGetUniformLocation(sceneShader->GetProgram(), "terrainHighBoundry"), (heightMap->GetHeightMapSize().y) / 8 * 4.5f);

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

void Renderer::DrawWater()
{
	glDisable(GL_CULL_FACE);
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox_Planet);

	Vector3 hSize = heightMap->GetHeightMapSize();

	modelMatrix =
		Matrix4::Translation(Vector3(hSize.x * 0.5f, hSize.y / 8.0f * 1.75f, hSize.z * 0.5f)) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(-90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();

	waterQuad->Draw();

	glEnable(GL_CULL_FACE);
	textureMatrix.ToIdentity();
}