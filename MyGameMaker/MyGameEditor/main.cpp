#include <iostream>

//assimp
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>

//imgui and panels
#include "MyGUI.h"

//opengl
#include "MyGameEngine/Renderer.h"

//engine
#include "MyGameEngine/Engine.h"

//modules
#include "MyGameEngine/MyWindow.h"
#include "MyGameEngine/Input.h"

//engine core
#include "MyGameEngine/Texture.h"
#include "MyGameEngine/Shader.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Scene.h"

//utils
#include "MyGameEngine/PerfTimer.h"

using namespace std;

int main(int argc, char* argv[]) {
	//start engine
	Engine& engine = Engine::Instance();
	engine.Awake();
	engine.Start();

	//strat myGUI
	MyGUI& gui = MyGUI::Instance();
	gui.Awake(engine.window->windowPtr(), engine.window->contextPtr());
	engine.input->SetEventProcessor(&gui);

	//Start Editor Application
	LOG(LogType::LOG_CHANGEENV, "-------------- Application Start --------------");

	//time
	const int maxFPS = 240;
	const float frameDelay = 1000.0f / maxFPS;  // Duraci�n m�nima de cada frame en milisegundos (5 ms)
	float fps = 0;
	double dT = 0.0f;
	PerfTimer timer;
	engine.SetFps(&fps);

	//game loop
	while (engine.input->GetWindowEvent(WE_QUIT) != true) {

		//time start
		timer.Start();

		//Preupdate
		engine.PreUpdate();

		//Update
		engine.Update(dT);

		//draw/Postupdate
		engine.Draw();

		gui.render();

		engine.PostUpdate(); //mirar de acabar de posar tots els draws aqui

		//time control
		//current time
		double frameTime = timer.ReadMs();  // Tiempo que tard� el frame en ms
		if (frameTime < frameDelay)
		{
			uint32 ms = frameDelay - frameTime;
			timer.Delay(ms);  // Esperar el tiempo restante
		}
		dT = timer.ReadMs() / 1000.0;
		fps = 1000.0f / (frameTime + (frameTime < frameDelay ? (frameDelay - frameTime) : 0));

	}

	return 0;
}