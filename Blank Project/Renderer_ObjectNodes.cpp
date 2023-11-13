#include "Renderer.h"

bool Renderer::ManageSceneNodes()
{
	
	Vector3 heightMapSize = heightMap->GetHeightMapSize();
	for (int i = 0; i < 5; i++)
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		s->SetTransform(Matrix4::Translation(Vector3(heightMapSize.x * 0.5f, heightMapSize.y * 1.2f, heightMapSize.z * 0.5f + 100.0f + 5000.0f * i)));
		s->SetModelScale(Vector3(1000.0f, 1000.0f, 1000.0f));
		s->SetBoundingRadius(1000.0f);
		s->SetMesh(rockModel1);
		s->SetTexture(rockTexture1);
		s->SetBump(rockBump1);
		planetSurfaceRoot->AddChild(s);
	}

	return true;
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	
	if (frameFrustum.InsideFrustum(*from))
	{
		//std::cout << "here";
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
	BindShader(modelShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(modelShader->GetProgram(), "diffuseTex"), 0);
	glEnable(GL_CULL_FACE);
	for (const auto& i : nodeList)
	{
		DrawNodes(i);
	}
	for (const auto& i : transparentNodeList)
	{
		DrawNodes(i);
	}
	glEnable(GL_CULL_FACE);
}

void Renderer::DrawNodes(SceneNode* n)
{
	if (n->GetMesh())
	{
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
		

		glUniform3fv(glGetUniformLocation(modelShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

		//UpdateShaderMatrices();
		SetShaderLight(*globalSceneLight);
		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}