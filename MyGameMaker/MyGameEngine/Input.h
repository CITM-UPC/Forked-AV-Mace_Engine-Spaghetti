#ifndef __INPUT_H__
#define __INPUT_H__

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
//#define LAST_KEYS_PRESSED_BUFFER 50

#include <SDL2/SDL_events.h>
#include "types.h"

class IEventProcessor {
public:
	virtual void processEvent(const SDL_Event& event) = 0;
};

struct SDL_Rect;

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input
{

public:

	Input();

	// Destructor
	virtual ~Input();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouseButtons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(EventWindow ev);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

	void SetEventProcessor(IEventProcessor* processor) {
		event_processor = processor;
	}

	void GetMouseWheel(int& y) {
		y = mouseWheel;
	}

	void GetMouseWheelMotion(bool& scrolling) {
		scrolling = mouseWheelScrolling;
	}

	void ActivateTextInput(bool activate = true) {
		if (activate)
			SDL_StartTextInput();
		else
			SDL_StopTextInput();
	}



private:
	bool windowEvents[WE_COUNT];
	KeyState*	keyboard;
	KeyState mouseButtons[NUM_MOUSE_BUTTONS];
	int	mouseMotionX;
	int mouseMotionY;
	int mouseX;
	int mouseY;
	int mouseWheel;
	bool mouseWheelScrolling;
	IEventProcessor* event_processor = nullptr;
};



#endif // __INPUT_H__