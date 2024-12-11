#include "BottomPanel.h"
#include "../MyGameEngine/Engine.h"
#include "../MyGameEngine/MyWindow.h"
#include "../MyGameEngine/types.h"
#include <imgui.h>

BottomPanel::BottomPanel(std::string name) : Panel(name, WINDOW_WIDTH, 200)
{
    console = new PanelConsole("Console");
    assets = new PanelAssets("Assets");
    SwitchState();
}

BottomPanel::~BottomPanel()
{
    delete console;
    delete assets;
}

bool BottomPanel::Draw()
{
    ImGui::SetNextWindowSize(ImVec2(Engine::Instance().window->width(), height));
    ImGui::SetNextWindowPos(ImVec2(0, Engine::Instance().window->height() - height));

    ImGui::Begin("Bottom Panel", &showWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginTabBar("BottomTabs"))
    {
        if (ImGui::BeginTabItem("Console"))
        {
            currentTab = 0;
            console->isActiveTab = true;
            assets->isActiveTab = false;
            console->Draw();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Assets"))
        {
            currentTab = 1;
            console->isActiveTab = false;
            assets->isActiveTab = true;
            assets->Draw();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    return true;
}