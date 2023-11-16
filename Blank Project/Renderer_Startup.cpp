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
		l.SetPosition(pointLightPositions[i]);
		
		l.SetColour(pointLightColours[i]);
		l.SetRadius(radius);
		pointLightStarts.push_back(2 * PI * (float)(rand() / (float)RAND_MAX));
	}
	pointLightCycle = 0.0f;
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
		int x = (int)round(heightMapCentre + heightMapRadius * cos(angle));
		int z = (int)round(heightMapCentre + heightMapRadius * sin(angle));

		float y = heightMap->GetHeightAtCoord(x, z);

		if (y != NULL && y < heightMapSize.y / 8 * 2)
		{
			float rotationX = (float)((rand() % (lbR - ubR + 1)) + lbR);
			float rotationY = (float)((rand() % (lbR - ubR + 1)) + lbR);

			for (int t = 0; t < 3; t++)
			{
				float lbS = lowerBounds[t];
				float ubS = upperBounds[t];
				scaleVectors[t] = lbS + (ubS - lbS) * (((float)rand()) / (float)RAND_MAX);
			}

			SceneNode* s = new SceneNode
			(
				Matrix4::Translation(Vector3((float)x * 16, (float)y, (float)z * 16)) * Matrix4::Rotation(rotationY, Vector3(0, 1, 0)),
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
		float heightMapRadius = ((heightMapSize.x / 16) / 2) * (0.25f) + ((heightMapSize.x / 16) / 2) * (1.0f / totalRocks * i) * 1.5f;
		float angle = (2 * PI) / 20 * i * 0.85f + (0.32f * PI);
		int x = (int)round(heightMapCentre + heightMapRadius * cos(angle));
		int z = (int)round(heightMapCentre + heightMapRadius * sin(angle));

		float y = heightMap->GetHeightAtCoord(x, z);

		if (y != NULL && y > heightMapSize.y / 8 * 2 && y < heightMapSize.y / 8 * 4)
		{
			float rotationX = (float)((rand() % (lbR - ubR + 1)) + lbR);
			float rotationY = (float)((rand() % (lbR - ubR + 1)) + lbR);

			for (int t = 0; t < 3; t++)
			{
				float lbS = lowerBounds[t];
				float ubS = upperBounds[t];
				scaleVectors[t] = lbS + (ubS - lbS) * (((float)rand()) / (float)RAND_MAX);
			}

			SceneNode* s = new SceneNode
			(
				Matrix4::Translation(Vector3((float)x * 16, (float)y, (float)z * 16)) * Matrix4::Rotation(rotationY, Vector3(0, 1, 0)),
				Vector3(scaleVectors[0], scaleVectors[1], scaleVectors[2]),
				500.0f,
				crystalModel1,
				crystalTexture1,
				crystalBump1
			);

			Vector4 colour = Vector4(
				0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX) * 0.25f,
				0.90f);

			//colour = Vector4(1, 1, 1, 1);

			s->SetColour(Vector4(colour.x * 3.0f, colour.y * 3.0f, colour.z * 3.0f, colour.w));
			pointLightPositions.push_back(Vector3((float)x * 16, (float)y, (float)z * 16));
			pointLightColours.push_back(Vector4(colour.x, colour.y, colour.z,1.0f));
			planetSurfaceRoot->AddChild(s);
		}
	}

	return true;
}

bool Renderer::SetUpMainPlanet()
{
	
	planetTexture = SOIL_load_OGL_texture(TEXTUREDIR"Mars.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	planetCloudTexture = SOIL_load_OGL_texture(TEXTUREDIR"Planet_Sky.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	planetBump = SOIL_load_OGL_texture(TEXTUREDIR"EmptyBump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(planetTexture, false);
	SetTextureRepeating(planetCloudTexture, true);
	SetTextureRepeating(planetBump, false);

	if (!planetTexture) return false;
	if (!planetCloudTexture) return false;
	if (!planetBump) return false;
	planet = new SceneNode();
	planet->SetBoundingRadius(1000.0f);
	planet->SetModelScale(Vector3(1000.0f, 1000.0f, 1000.0f));
	planet->SetMesh(planetModel);
	planet->SetTexture(planetTexture);
	planet->SetTexture2(planetCloudTexture);
	planet->SetBump(planetBump);
	planet->SetTransform(Matrix4::Translation(Vector3(50000.0f, 1000.0f, 0)) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	planetCycle = 0.0f;
	planet->SetTag(SCENENODETAG_PLANET);
	spaceRoot->AddChild(planet);
	return true;
}

bool Renderer::SetUpSun()
{

	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"Sun.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(sunTexture, false);
	if (!sunTexture) return false;

	sun = new SceneNode();
	sun->SetBoundingRadius(5000.0f);
	sun->SetModelScale(Vector3(5000.0f, 5000.0f, 5000.0f));
	sun->SetMesh(planetModel);
	sun->SetTexture(sunTexture);
	sun->SetTexture2(sunTexture);
	sun->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	sun->SetTag(SCENENODETAG_PLANET);
	sun->SetLightingInfluence(0);

	spaceRoot->AddChild(sun);
	return true;
}