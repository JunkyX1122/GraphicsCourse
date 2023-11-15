#include "Renderer.h"

bool Renderer::SetUpTerrain()
{
	heightMap = new HeightMap(TEXTUREDIR"terrain_3.png", 128.0f, 8);
	

	seaBedTexture = SOIL_load_OGL_texture(TEXTUREDIR"Wet_Soil_Shoeprints_albedo.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	seaBedBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Wet_Soil_Shoeprints_normal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	groundTexture = SOIL_load_OGL_texture(TEXTUREDIR"Rock_03_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Rock_03_NRM.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	highGroundTexture = SOIL_load_OGL_texture(TEXTUREDIR"Dirty_Grass_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	highGroundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Dirty_Grass_NRM2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(seaBedTexture, true);
	SetTextureRepeating(seaBedBumpMap, true);

	SetTextureRepeating(groundTexture, true);
	SetTextureRepeating(groundBumpMap, true);

	SetTextureRepeating(highGroundTexture, true);
	SetTextureRepeating(highGroundBumpMap, true);

	if (!heightMap) return false;

	if (!seaBedTexture) return false;
	if (!seaBedBumpMap) return false;

	if (!groundTexture) return false;
	if (!groundBumpMap) return false;

	if (!highGroundTexture) return false;
	if (!highGroundBumpMap) return false;

	heightMapSize = heightMap->GetHeightMapSize();
	return true;
}

bool Renderer::SetUpWater()
{
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(waterTex, true);
	if (!waterTex) return false;

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	if (!reflectShader->LoadSuccess()) return false;

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	return true;
}

bool Renderer::SetUpSkybox()
{
	skyBox_Planet =
		SOIL_load_OGL_cubemap(
			TEXTUREDIR"rusted_west_new.JPG",
			TEXTUREDIR"rusted_east_new.JPG",
			TEXTUREDIR"rusted_up_new.JPG",
			TEXTUREDIR"rusted_down_new.JPG",
			TEXTUREDIR"rusted_south_new.JPG",
			TEXTUREDIR"rusted_north_new.JPG",
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			0);
	if (!skyBox_Planet) return false;

	skyBox_Space =
		SOIL_load_OGL_cubemap(
			TEXTUREDIR"space_west.png",
			TEXTUREDIR"space_east.png",
			TEXTUREDIR"space_up.png",
			TEXTUREDIR"space_down.png",
			TEXTUREDIR"space_south.png",
			TEXTUREDIR"space_north.png",
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			0);
	if (!skyBox_Space) return false;

	skybox_Planet_Shader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	if (!skybox_Planet_Shader->LoadSuccess()) return false;

	return true;
}

bool Renderer::SetUpPointLights()
{
	pointLights = new Light[LIGHT_NUM];
	float radius = 5000.0f;
	for (int i = 0; i < pointLightPositions.size(); i++)
	{
		Light& l = pointLights[i];
		l.SetPosition(pointLightPositions[i] + Vector3(0,500.0f,0) - Vector3(radius, radius, radius) / 4);

		l.SetColour(Vector4(
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.SetRadius(radius);
	}
	return true;
}

bool Renderer::SetUpRocks()
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

			for (int t = 0; t < 3; t++)
			{
				int lbS = lowerBounds[t], ubS = upperBounds[t];
				scaleVectors[t] = (rand() % (lbS - ubS + 1)) + lbS;
			}

			SceneNode* s = new SceneNode
			(
				Matrix4::Translation(Vector3(x * 16, y, z * 16)) * Matrix4::Rotation(rotationY, Vector3(0, 1, 0)),
				Vector3(scaleVectors[0], scaleVectors[1], scaleVectors[2]),
				scaleVectors[1] * 1.3f,
				rockModel1,
				rockTexture1,
				rockBump1
			);
			s->SetColour(Vector4(1, 1, 1, 1));
			planetSurfaceRoot->AddChild(s);
		}
	}
	return true;
}

bool Renderer::SetUpCrystals()
{
	crystalModel1 = Mesh::LoadFromMeshFile("Crystalsv03.msh");
	crystalTexture1 = SOIL_load_OGL_texture(TEXTUREDIR"TranslucentCrystal01_Emission.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	crystalBump1 = SOIL_load_OGL_texture(TEXTUREDIR"EmptyBump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(crystalTexture1, true);
	SetTextureRepeating(crystalBump1, true);

	if (!crystalModel1) return false;
	if (!crystalTexture1) return false;
	if (!crystalBump1) return false;

	Vector3 heightMapSize = heightMap->GetHeightMapSize();
	const int perRotation = 20;
	const int rotationCycles = 5;
	const int totalRocks = perRotation * rotationCycles;

	int lbR = 0, ubR = 360;

	float scaleVectors[3];
	float lowerBounds[3] = { 200, 200, 200 };
	float upperBounds[3] = { 400, 400, 400 };


	for (int i = 0; i < totalRocks; i++)
	{
		float heightMapCentre = (heightMapSize.x / 16) / 2;
		float heightMapRadius = ((heightMapSize.x / 16) / 2) * 0.25f + ((heightMapSize.x / 16) / 2) * (1.0f / totalRocks * i) * 2;
		float angle = (2 * PI) / 20 * i * 0.85f + (0.32f * PI);
		int x = round(heightMapCentre + heightMapRadius * cos(angle));
		int z = round(heightMapCentre + heightMapRadius * sin(angle));

		float y = heightMap->GetHeightAtCoord(x, z);

		if (y != NULL && y > heightMapSize.y / 8 * 2 && y < heightMapSize.y / 8 * 4)
		{
			float rotationX = (rand() % (lbR - ubR + 1)) + lbR;
			float rotationY = (rand() % (lbR - ubR + 1)) + lbR;

			for (int t = 0; t < 3; t++)
			{
				int lbS = lowerBounds[t], ubS = upperBounds[t];
				scaleVectors[t] = (rand() % (lbS - ubS + 1)) + lbS;
			}

			SceneNode* s = new SceneNode
			(
				Matrix4::Translation(Vector3(x * 16, y, z * 16)) * Matrix4::Rotation(rotationY, Vector3(0, 1, 0)),
				Vector3(scaleVectors[0], scaleVectors[1], scaleVectors[2]),
				scaleVectors[1] * 1.3f,
				crystalModel1,
				crystalTexture1,
				crystalBump1
			);
			
			s->SetColour(Vector4(1, 1, 1, 0.75f));
			pointLightPositions.push_back(Vector3(x * 16, y, z * 16));
			planetSurfaceRoot->AddChild(s);
		}
	}
	return true;
}
