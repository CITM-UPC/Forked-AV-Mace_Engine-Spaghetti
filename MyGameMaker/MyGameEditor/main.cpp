//imgui and panels
#include "MyGUI.h"

#include "MyGameEngine/Scene.h"
#include "MyGameEngine/Scene.cpp"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/BoundingBox.h"


//engine
#include "MyGameEngine/Engine.h"

//modules
#include "MyGameEngine/MyWindow.h"
#include "MyGameEngine/Input.h"

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
	LOG(LogType::LOG_CHANGE_ENV, "-------------- Application Start --------------");

	//time
	const int maxFPS = 240;
	const float frameDelay = 1000.0f / maxFPS;  // Duración mínima de cada frame en milisegundos (5 ms)
	float fps = 0;
	double dT = 0.0f;
	PerfTimer timer;
	engine.SetFPS(&fps);

	//game loop
	while (engine.quit != true) {

		//time start
		timer.Start();

		engine.PreUpdate();

		engine.Update(dT);

		engine.PostUpdate();
		
		gui.render();

		engine.SwapBuffers();

		//time control
		//current time
		double frameTime = timer.ReadMs();  // Tiempo que tardó el frame en ms
		if (frameTime < frameDelay)
		{
			uint32 ms = frameDelay - frameTime;
			timer.Delay(ms);  // Esperar el tiempo restante
		}
		dT = timer.ReadMs() / 1000.0;
		fps = 1000.0f / (frameTime + (frameTime < frameDelay ? (frameDelay - frameTime) : 0));

	}

	gui.CleanUp();
	engine.CleanUp();



	return 0;

}