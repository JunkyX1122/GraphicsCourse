#include "Renderer.h"

bool Renderer::ManagePlanetSurfaceSceneNodes()
{
	rockModel1 = Mesh::LoadFromMeshFile("Rock_02_LOD0 .msh");
	rockTexture1 = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockBump1 = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(rockTexture1, true);
	SetTextureRepeating(rockBump1, true);

	if (!rockModel1) return false;
	if (!rockTexture1) return false;
	if (!rockBump1) return false;

	Vector3 heightMapSize = heightMap->GetHeightMapSize();
	const int perRotation = 20;
	const int rotationCycles = 9;
	const int totalRocks = perRotation * rotationCycles;

	int lbR = 0, ubR = 360;

	float scaleVectors[3];
	float lowerBounds[3] = { 700, 700, 700 };
	float upperBounds[3] = { 900, 1700, 900 };


	for (int i = 0; i < totalRocks; i++)
	{
		float heightMapCentre = (heightMapSize.x / 16) / 2;
		float heightMapRadius = ((heightMapSize.x / 16) / 2) * (1.0f / totalRocks * i) * 2;
		float angle = (2 * PI) / 20 * i * 0.85f;
		int x = round(heightMapCentre + heightMapRadius * cos(angle));
		int z = round(heightMapCentre + heightMapRadius * sin(angle));

		float y = heightMap->GetHeightAtCoord(x, z);

		if (y != NULL && y < heightMapSize.y / 8 * 2)
		{
			float rotationX = (rand() % (lbR - ubR + 1)) + lbR;
			float rotationY = (rand() % (lbR - ubR + 1)) + lbR;
			

			SceneNode* s = new SceneNode();

			s->SetTransform(
				Matrix4::Translation(Vector3(x * 16, y, z * 16)) *
				Matrix4::Rotation(rotationY, Vector3(0, 1, 0)));

			
			for (int t = 0; t < 3; t++)
			{
				int lbS = lowerBounds[t], ubS = upperBounds[t];
				scaleVectors[t] = (rand() % (lbS - ubS + 1)) + lbS;
			}
			s->SetModelScale(Vector3(scaleVectors[0], scaleVectors[1], scaleVectors[2]));
			s->SetBoundingRadius(scaleVectors[1] * 1.1f);
			s->SetMesh(rockModel1);
			s->SetTexture(rockTexture1);
			s->SetBump(rockBump1);
			planetSurfaceRoot->AddChild(s);
		}
	}

	return true;
}

bool Renderer::ManageSpaceSceneNodes()
{
	planetModel = Mesh::LoadFromMeshFile("Mars.msh");
	planetTexture = SOIL_load_OGL_texture(TEXTUREDIR"Mars.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//planetBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!planetModel) return false;
	if (!planetTexture) return false;
	/*
	SceneNode* s = new SceneNode();
	s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));


	s->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));

	s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	s->SetBoundingRadius(100.0f);
	s->SetMesh(rockModel1);
	s->SetTexture(rockTexture1);
	s->SetBump(rockBump1);
	planetSurfaceRoot->AddChild(s);
	return true;
	//if (!rockBump1) return false;*/
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