#include "MyGUI.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include "PanelConsole.h"
#include "PanelMenu.h"
#include "PanelInspector.h"
#include "PanelHierarchy.h"
#include "PanelConfiguration.h"
#include "PanelAssets.h"

MyGUI::~MyGUI() {
	
}

void MyGUI::Awake(SDL_Window* window, void* context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init();

	_console = new PanelConsole("Console");
	_menu = new PanelMenu("Menu");
	_inspector = new PanelInspector("Inspector");
	_hierarchy = new PanelHierarchy("Hierarchy");
	_configuration = new PanelConfiguration("Configuration");
	_assets = new PanelAssets("Assets");

	addPanel(_console);
	addPanel(_menu);
	addPanel(_inspector);
	addPanel(_hierarchy);
	addPanel(_configuration);
	addPanel(_assets);
}

void MyGUI::render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	
	for (const auto& panel : _panels)
	{
		if (panel->GetState() == false)
			continue;
		
		panel->Draw();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void MyGUI::addPanel(Panel* panel)
{
	_panels.push_back(panel);
}

void MyGUI::CleanUp()
{
	for (auto panel : _panels)
	{
		delete panel;
	}
	_panels.clear();

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
