#pragma once

#include <list>

#include "MyGameEngine/input.h"
#include "Panel.h"

class PanelConsole;
class PanelMenu;
class PanelInspector;
class PanelHierarchy;
class PanelConfiguration;
class PanelAssets;

class MyGUI : public IEventProcessor
{
	std::list<Panel*> _panels;

public:
	static MyGUI& Instance() {
		static MyGUI instance;  // La instancia única de la clase Engine
		return instance;
	}

	// Evita la copia y asignación del singleton
	MyGUI(const MyGUI&) = delete;
	MyGUI& operator=(const MyGUI&) = delete;
	MyGUI(MyGUI&&) noexcept = delete;

	void Awake(SDL_Window* window, void* context);

	void render();

	void processEvent(const SDL_Event& event) override;

	PanelConsole& console() const { return *_console; }
	PanelInspector& inspector() const { return *_inspector; }
	PanelHierarchy& hierarchy() const { return *_hierarchy; }
	PanelConfiguration& configuration() const { return *_configuration; }
	PanelAssets& assets() const { return *_assets; }

	void CleanUp();

private:
	MyGUI() = default;
	~MyGUI();

	//List of panels
	PanelConsole* _console = nullptr;
	PanelMenu* _menu = nullptr;
	PanelInspector* _inspector = nullptr;
	PanelHierarchy* _hierarchy = nullptr;
	PanelConfiguration* _configuration = nullptr;
	PanelAssets* _assets = nullptr;
	
	void addPanel(Panel* panel);
};