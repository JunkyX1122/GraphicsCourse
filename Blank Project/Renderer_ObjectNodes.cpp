#include "Renderer.h"

bool Renderer::ManageSceneNodes()
{
	planetSurfaceRoot = new SceneNode();
	spaceRoot = new SceneNode();

	return true;
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (!from)
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
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

		/*
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), texture);
		*/
		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}