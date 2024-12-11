#include "Input.h"

#include <SDL2/SDL.h>
#include <iostream>

#include "Engine.h"
#include "MyWindow.h"
#include "Scene.h"
#include "types.h"


#define MAX_KEYS 300

Input::Input()
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake()
{

	LOG(LogType::LOG_INFO, "# Initializing Input Subsystem...");

	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{

	mouseWheelScrolling = false;

	static SDL_Event event;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouseButtons[i] == KEY_DOWN)
			mouseButtons[i] = KEY_REPEAT;

		if(mouseButtons[i] == KEY_UP)
			mouseButtons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		SDL_KeyboardEvent& key_event = event.key;
		if (event_processor) {
			event_processor->processEvent(event);
		}

		switch(event.type)
		{
			case SDL_QUIT:
				Engine::Instance().quit = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;

					case SDL_WINDOWEVENT_SIZE_CHANGED:
						Engine::Instance().window->setWidth(event.window.data1);
						Engine::Instance().window->setHeight(event.window.data2);
						break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouseButtons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouseButtons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
				mouseMotionX = event.motion.xrel;
				mouseMotionY = event.motion.yrel;
				mouseX = event.motion.x;
				mouseY = event.motion.y;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;

			case SDL_MOUSEWHEEL:
				mouseWheelScrolling = true;
				mouseWheel = event.wheel.y;
				break;

			case SDL_DROPFILE:
				std::string filename = event.drop.file;
				if (filename.ends_with(".png") || filename.ends_with(".dds")) {
					Engine::Instance().scene->loadTextureByPath(event.drop.file);
				}
				else if (filename.ends_with(".fbx") || filename.ends_with(".FBX")) {
					Engine::Instance().scene->loadGameObjectByPath(event.drop.file);
				}
				SDL_free(event.drop.file);
				break;
		}

	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_Quit();
	return true;
}


bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}

void Input::GetMousePosition(int& x, int& y)
{
	x = mouseX;
	y = mouseY;
}

void Input::GetMouseMotion(int& x, int& y)
{
	x = mouseMotionX;
	y = mouseMotionY;
}
