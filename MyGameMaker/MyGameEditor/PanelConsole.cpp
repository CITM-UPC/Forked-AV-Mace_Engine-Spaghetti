#include "PanelConsole.h"

#include <imgui.h>

#include "../MyGameEngine/Engine.h"
#include "../MyGameEngine/MyWindow.h"
#include "../MyGameEngine/types.h"


PanelConsole::PanelConsole(std::string name) : Panel(name, WINDOW_WIDTH, 200)
{
	SwitchState();
}

PanelConsole::~PanelConsole() {}

bool PanelConsole::Draw()
{
	if (isActiveTab) {
		DrawContents();
	}
	return true;
}

void PanelConsole::DrawContents()
{
	ImVec4 color = ImVec4(1, 1, 1, 1);
	for (size_t i = 0; i < Engine::Instance().GetLogs().size(); i++)
	{
		switch (Engine::Instance().GetLogs()[i].type)
		{
		case LogType::LOG_INFO:
			color = ImVec4(1, 1, 1, 1);
			break;
		case LogType::LOG_APPINFO:
			color = ImVec4(1, 0.5, 1, 1);
			break;
		case LogType::LOG_CHANGE_ENV:
			color = ImVec4(0.5, 1, 1, 1);
			break;
		case LogType::LOG_WARNING:
			color = ImVec4(1, 1, 0.5, 1);
			break;
		};

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::Text(Engine::Instance().GetLogs()[i].message.c_str());
		ImGui::PopStyleColor();
	}
}