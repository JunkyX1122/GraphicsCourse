#include "Renderer.h"

bool Renderer::ManagePlanetSurfaceSceneNodes()
{
	if (!SetUpRocks()) return false;
	if (!SetUpCrystals()) return false;

	return true;
}

bool Renderer::ManageSpaceSceneNodes()
{
	planetModel = Mesh::LoadFromMeshFile("Mars.msh");
	if (!planetModel) return false;

	if (!SetUpMainPlanet()) return false;
	if (!SetUpSun()) return false;
	if (!SetUpAsteroids()) return false;
	return true;
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}

	}
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(),
		transparentNodeList.rend(),
		SceneNode::CompareByCameraDistance);

	std::sort(nodeList.rbegin(),
		nodeList.rend(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes()
{
	for (const auto& i : nodeList)
	{
		DrawNodes(i);
	}
	for (const auto& i : transparentNodeList)
	{
		DrawNodes(i);
	}
	
}

void Renderer::DrawNodes(SceneNode* n)
{
	if (n->GetMesh())
	{
		glEnable(GL_CULL_FACE);
		int tag = n->GetTag();
		if (tag == -1)
		{
			BindShader(modelShader);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(modelShader->GetProgram(), "modelMatrix"), 1, false, model.values);

			glUniform1i(glGetUniformLocation(modelShader->GetProgram(), "diffuseTex"), 0);
			basicTexture = n->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, basicTexture);
			
			glUniform1i(glGetUniformLocation(modelShader->GetProgram(), "bumpTex"), 1);
			basicBump = n->GetBump();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, basicBump);


			glUniform3fv(glGetUniformLocation(modelShader->GetProgram(), "cameraPos"),			1, (float*)&camera->GetPosition());
			glUniform4fv(glGetUniformLocation(modelShader->GetProgram(), "colourCorrection"),	1, (float*)&n->GetColour());

			SetShaderLight(*globalSceneLight);
			n->Draw(*this);
		}
		else
		{
			bool easterEggCheck = easterEgg && tag == SCENENODETAG_SUN;
			if (easterEggCheck)
			{
				n->SetMesh(quad);
				glDisable(GL_CULL_FACE);
			}
			else
			{
				n->SetMesh(planetModel);
			}
			Shader* shaderInUse = planetShader;
			BindShader(shaderInUse);
			UpdateShaderMatrices();
			
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(shaderInUse->GetProgram(), "modelMatrix"), 1, false, model.values);

			glUniform1i(glGetUniformLocation(shaderInUse->GetProgram(), "diffuseTex"), 0);
			basicTexture = n->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, easterEggCheck ? easterEggTex : basicTexture);

			glUniform1i(glGetUniformLocation(shaderInUse->GetProgram(), "bumpTex"), 1);
			basicBump = n->GetBump();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, easterEggCheck ? easterEggTex : basicBump);

			glUniform1i(glGetUniformLocation(shaderInUse->GetProgram(), "diffuseTex2"), 2);
			basicTexture = n->GetTexture2();
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, easterEggCheck ? easterEggTex : basicTexture);

			glUniform3fv(glGetUniformLocation(shaderInUse->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
			glUniform1i(glGetUniformLocation(shaderInUse->GetProgram(), "lightInfluence"), n->GetLightingInfluence());

			float timerAlt = tag == 0 ? planetCycle * 0.1f * 0.25f * 0.25f : -planetCycle * 0.1f * 0.5f;
			glUniform4fv(glGetUniformLocation(shaderInUse->GetProgram(), "transformValues"), 1, (float*)&Vector4(1.0f, 1.0f, timerAlt, timerAlt));

			glUniform1i(glGetUniformLocation(shaderInUse->GetProgram(), "easterEgg"), easterEggCheck ? 1 : 0);
			glUniform1i(glGetUniformLocation(shaderInUse->GetProgram(), "easterEggV"), easterEggCheck ? 1 : 0);

			SetShaderLight(*globalSceneLight);
			n->Draw(*this);			
		}
		glDisable(GL_CULL_FACE);
	}
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}