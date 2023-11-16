#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	//Window w("Make your own project!", 1280, 720, false);
	Window w("Make your own project!", 1920, 1080, true);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0))
		{
			renderer.SetSceneType(renderer.GetSceneType() == 0 ? 1 : 0);
			if (renderer.GetSceneType() == 0) renderer.GetCamera()->LockFreeMovement();
			if (renderer.GetSceneType() == 1) renderer.GetCamera()->UnlockFreeMovement();

		}
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) 
		{
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}