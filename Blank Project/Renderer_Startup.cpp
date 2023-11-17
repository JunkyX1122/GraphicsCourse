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
				1700.0f * 1.3f,
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
				1300.0f,
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
	planet->SetTransform(Matrix4::Translation(Vector3(50000.0f, 0, 0)) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	planet->SetModelScale(Vector3(1000.0f, 1000.0f, 1000.0f));
	planet->SetMesh(planetModel);
	planet->SetTexture(planetTexture);
	planet->SetTexture2(planetCloudTexture);
	planet->SetBump(planetBump);
	
	planetCycle = 0.0f;
	planet->SetTag(SCENENODETAG_PLANET);
	spaceRoot->AddChild(planet);
	return true;
}

bool Renderer::SetUpSun()
{

	easterEggTex = SOIL_load_OGL_texture(TEXTUREDIR"OIP.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(easterEggTex, true);

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
	sun->SetTag(SCENENODETAG_SUN);
	sun->SetLightingInfluence(0);

	spaceRoot->AddChild(sun);
	return true;
}

bool Renderer::SetUpAsteroids()
{
	asteroidParent = new SceneNode();
	spaceRoot->AddChild(asteroidParent);
	asteroidModel1 = Mesh::LoadFromMeshFile("Rock_02_LOD0 .msh");
	asteroidTexture1 = SOIL_load_OGL_texture(TEXTUREDIR"Asteroids.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	asteroidBump1 = SOIL_load_OGL_texture(TEXTUREDIR"Asteroids_Bump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(asteroidTexture1, true);
	SetTextureRepeating(asteroidBump1, true);

	if (!asteroidModel1) return false;
	if (!asteroidTexture1) return false;
	if (!asteroidBump1) return false;

	const int perRotation = 50;

	int lbR = 0, ubR = 360;

	float scaleVectors[3];
	float lowerBounds[3] = { 100, 100, 100 };
	float upperBounds[3] = { 200, 200, 200 };


	for (int i = 0; i < perRotation; i++)
	{
		float randDev = 0.4f + (0.6f - 0.4f) * (((float)rand()) / (float)RAND_MAX);
		float distance = 50000.0f * randDev;
		float angle = (2 * PI) / perRotation * i;
		float anglePitch = (PI * 0.5f) * (0.1f - 0.2 * (((float)rand()) / (float)RAND_MAX));
		float x = distance * cos(angle) * cos(anglePitch);
		float y = distance * sin(anglePitch);
		float z = distance * sin(angle) * cos(anglePitch);
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
			Matrix4::Translation(Vector3((float)x, (float)y, (float)z)) * Matrix4::Rotation(rotationY, Vector3(0, 1, 0)),
			Vector3(scaleVectors[0], scaleVectors[1], scaleVectors[2]),
			700.0f,
			rockModel1,
			rockTexture1,
			rockBump1
		);
		Vector3 rotSpd = Vector3(1 - 2 * (((float)rand()) / (float)RAND_MAX), 1 - 2 * (((float)rand()) / (float)RAND_MAX), 1 - 2 * (((float)rand()) / (float)RAND_MAX));
		s->SetRotationSpeed(rotSpd);
		s->SetColour(Vector4(1, 1, 1, 1));
		asteroidParent->AddChild(s);	
	}
	return true;
}



bool Renderer::SetUpCamera()
{
	introFlag = false;
	introTimer = 0.0f;
	cameraFOV = 90.0f;
	cameraTimer = 0.0f;
	cameraOrbitTimer = 0.0f;
	cameraAutoMoveType = 0;
	float bpm = 130.0f;
	float timesig = 4.0f;
	float barspm = bpm / timesig;
	timePerBar = 60.0f / barspm;
	
	// 60bpm = 1bps
	// 120bpm = 2bps
	// 130bpm = 2.1667bps
	

	camera = new Camera(-45.0f, 0.0f, Vector3(24174.8f, 3312.67f, 7907.53f));
	camera->SetRotation(Vector3(-3.28f, 165.19f, 0.0f));
	camera->SetRotationSetter(Vector3(-3.28f, 165.19f, 0.0f));
	storedCamPosition[1] = Vector3(50000.0f + 1500.0f,0,0);
	storedCamRotation[1] = Vector3(0,90.0f,0);

	camera->LockFreeMovement();
	
	float waitBars = 4.0f;
	float defaultTime = waitBars * timePerBar;
	AddCameraKeyFrame(Vector3(24174.8f, 3312.67f, 7907.53f), Vector3(-3.28f , 165.19f , 0.0f		), defaultTime); // 0
	AddCameraKeyFrame(Vector3(25776.7f, 2967.56f, 11433.8f), Vector3(5.26f  , 225.95f , -5.0f		), defaultTime);
	AddCameraKeyFrame(Vector3(27447.6f, 2021.92f, 14204.2f), Vector3(12.26f , 133.829f, 2.5f		), defaultTime);
	AddCameraKeyFrame(Vector3(27666.3f, 2757.5f , 16105.7f), Vector3(-2.37f , 69.919f , 20.5		), defaultTime);
	AddCameraKeyFrame(Vector3(25303.6f, 3402.68f, 20467.8f), Vector3(-16.72f, 47.219f , 8.5f		), defaultTime);
	AddCameraKeyFrame(Vector3(25389.0f, 4724.11f, 22105.6f), Vector3(-18.68f, 57.0393f, 0.0f		), defaultTime);
	AddCameraKeyFrame(Vector3(23345.5f, 3042.83f, 25329.4f), Vector3(19.68f , 56.3394f, -2.5f		), defaultTime);

	AddCameraKeyFrame(Vector3(20325.0f, 5024.69f, 25561.1f), Vector3(-27.57f, 89.0293f , 7.0f		), defaultTime);
	AddCameraKeyFrame(Vector3(14930.9f, 1196.40f, 28449.7f), Vector3(-21.62f, 40.7293f , -16.5f		), defaultTime);
	AddCameraKeyFrame(Vector3(10220.0f, 686.524f, 26711.1f), Vector3(15.13f , -24.4407f, -21.0f		), defaultTime);

	AddCameraKeyFrame(Vector3(6408.33f, 1841.56f, 23910.8f), Vector3(-7.9f  , 33.5193f , 10.5f		), defaultTime);
	AddCameraKeyFrame(Vector3(3531.42f, 3057.34f, 17650.9f), Vector3(-40.17f, -53.7707f, 3.0f		), defaultTime);
	AddCameraKeyFrame(Vector3(14926.6f, 1826.32f, 2392.37f), Vector3(-15.67f, -70.3608f, 0.0f		), defaultTime);
	AddCameraKeyFrame(Vector3(20294.2f, 7576.22f, 4030.8f ), Vector3(-45.7f , -194.401f, 0.0f		), defaultTime);
	AddCameraKeyFrame(Vector3(22057.7f, 13146.3f, 14489.4f), Vector3(-69.57f, -250.401f, 0.0f		), defaultTime);
	//AddCameraKeyFrame(Vector3(), Vector3());
	//AddCameraKeyFrame(Vector3(), Vector3());

	cameraKeyFrameCount_Planet = cameraPositions_Planet.size();
	currentKeyFrame = 0;
	cameraAnimateSpeed = 0.0f;
	return true;
}
void Renderer::AddCameraKeyFrame(Vector3 pos, Vector3 rot, float timeToReach)
{
	cameraPositions_Planet.push_back(pos);
	cameraRotations_Planet.push_back(rot);
	cameraKeyTimes_Planet.push_back(timeToReach);
}