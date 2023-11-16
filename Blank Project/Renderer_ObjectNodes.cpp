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
	planetTexture = SOIL_load_OGL_texture(TEXTUREDIR"Mars.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	planetCloudTexture = SOIL_load_OGL_texture(TEXTUREDIR"EmptyBump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	planetBump = SOIL_load_OGL_texture(TEXTUREDIR"EmptyBump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(planetTexture, false);
	SetTextureRepeating(planetCloudTexture, true);
	SetTextureRepeating(planetBump, false);


	if (!planetModel) return false;
	if (!planetTexture) return false;
	if (!planetCloudTexture) return false;
	if (!planetBump) return false;
	planet = new SceneNode();
	planet->SetBoundingRadius(1000.0f);
	planet->SetModelScale(Vector3(5000.0f, 5000.0f, 5000.0f));
	planet->SetMesh(planetModel);
	planet->SetTexture(planetTexture);
	planet->SetBump(planetBump);
	planet->SetTransform(Matrix4::Translation(Vector3(50000.0f, 1000.0f, 0)) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	planetCycle = 0.0f;
	planet->SetTag(SCENENODETAG_PLANET);
	spaceRoot->AddChild(planet);
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
			
			BindShader(planetShader);
			UpdateShaderMatrices();
			
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(planetShader->GetProgram(), "modelMatrix"), 1, false, model.values);

			glUniform1i(glGetUniformLocation(planetShader->GetProgram(), "diffuseTex"), 0);
			basicTexture = n->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, basicTexture);

			glUniform1i(glGetUniformLocation(planetShader->GetProgram(), "bumpTex"), 1);
			basicBump = n->GetBump();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, basicBump);

			glUniform1i(glGetUniformLocation(planetShader->GetProgram(), "diffuseTex2"), 2);
			basicTexture = planetCloudTexture;
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, basicTexture);

			glUniform3fv(glGetUniformLocation(planetShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
			glUniform1f(glGetUniformLocation(planetShader->GetProgram(), "timer"), planetCycle);

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