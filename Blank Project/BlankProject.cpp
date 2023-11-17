#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	//Window w("Make your own project!", 1280, 720, false);
	Window w("Make your own project!", 1600, 900, false);
	//Window w("Make your own project!", 1920, 1080, true);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	Vector3 storedCamPosition[2];
	Vector3 storedCamRotation[2];
	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RETURN))
		{
			int currentScene = renderer.GetSceneType();
			int nextScene = (currentScene + 1) % 2;
			storedCamPosition[currentScene] = renderer.GetCamera()->GetPosition();
			storedCamRotation[currentScene] = renderer.GetCamera()->GetRotation();

			renderer.GetCamera()->SetPosition(storedCamPosition[nextScene]);
			renderer.GetCamera()->SetRotation(storedCamRotation[nextScene]);
			renderer.GetCamera()->SetPositionSetter(storedCamPosition[nextScene]);
			renderer.GetCamera()->SetRotationSetter(storedCamRotation[nextScene]);
			renderer.SetSceneType(currentScene == 0 ? 1 : 0);
			

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