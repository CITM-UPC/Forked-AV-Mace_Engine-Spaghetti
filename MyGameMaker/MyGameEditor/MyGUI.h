#pragma once

#include <list>
#include <vector>

#include "../MyGameEngine/input.h"
#include "Panel.h"
#include "BottomPanel.h"

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

	PanelInspector& inspector() const { return *_inspector; }
	PanelHierarchy& hierarchy() const { return *_hierarchy; }
	PanelConfiguration& configuration() const { return *_configuration; }

	PanelConsole& console() const { return *_bottomPanel->getConsole(); }
	PanelAssets& assets() const { return *_bottomPanel->getAssets(); }

	void CleanUp();

private:
	MyGUI() = default;
	~MyGUI();

	//List of panels
	PanelMenu* _menu = nullptr;
	BottomPanel* _bottomPanel = nullptr;
	PanelInspector* _inspector = nullptr;
	PanelHierarchy* _hierarchy = nullptr;
	PanelConfiguration* _configuration = nullptr;
	
	void addPanel(Panel* panel);
};