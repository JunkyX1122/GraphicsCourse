#include "Renderer.h"

bool Renderer::SetUpTerrain()
{
	heightMap = new HeightMap(TEXTUREDIR"terrain_3.png", 128.0f, 8);
	

	seaBedTexture = SOIL_load_OGL_texture(TEXTUREDIR"Wet_Soil_Shoeprints_albedo.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	seaBedBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Wet_Soil_Shoeprints_normal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	groundTexture = SOIL_load_OGL_texture(TEXTUREDIR"Rock_03_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Rock_03_NRM.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	highGroundTexture = SOIL_load_OGL_texture(TEXTUREDIR"Dirty_Grass_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	highGroundBumpMap = SOIL_load_OGL_texture(TEXTUREDIR"Dirty_Grass_NRM.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
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
	for (int i = 0; i < LIGHT_NUM; i++)
	{
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightMapSize.x, heightMapSize.y * 0.5f, rand() % (int)heightMapSize.z));

		l.SetColour(Vector4(
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.SetRadius(15000.0f);
	}
	return true;
}