#include "Renderer.h"



bool Renderer::SetUpShadows()
{
	shadowSceneShader = new Shader("shadowSceneVertex.glsl", "shadowSceneFragment.glsl");
	shadowShader = new Shader("shadowVertex.glsl", "shadowFragment.glsl");
	if (!shadowSceneShader->LoadSuccess()) return false;
	if (!shadowShader->LoadSuccess()) return false;
	return true;
}
void Renderer::DrawShadows()
{
	



	
}