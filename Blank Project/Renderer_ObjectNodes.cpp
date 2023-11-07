#include "Renderer.h"

bool Renderer::ManageSceneNodes()
{
	
	Vector3 heightMapSize = heightMap->GetHeightMapSize();
	for (int i = 0; i < 5; i++)
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(Vector3(heightMapSize.x * 0.5f, heightMapSize.y * 1.2f, heightMapSize.z * 0.5f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(10000.0f, 10000.0f, 10000.0f));
		s->SetBoundingRadius(10000.0f);
		s->SetMesh(quad);
		s->SetTexture(seaBedTexture);
		planetSurfaceRoot->AddChild(s);
	}

	return true;
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	
	if (frameFrustum.InsideFrustum(*from))
	{
		std::cout << "here";
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
	BindShader(basicShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "diffuseTex"), 0);
	glDisable(GL_CULL_FACE);
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

		
		glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(basicShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		basicTexture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, basicTexture);

		glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "useTexture"), basicTexture);
		

		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}