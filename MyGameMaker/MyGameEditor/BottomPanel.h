#pragma once
#include "Panel.h"
#include "PanelConsole.h"
#include "PanelAssets.h"

class BottomPanel : public Panel 
{
public:
    BottomPanel(std::string name);
    ~BottomPanel();

    bool Draw() override;

    PanelConsole* getConsole() const { return console; }
    PanelAssets* getAssets() const { return assets; }

private:
    PanelConsole* console;
    PanelAssets* assets;
    int currentTab = 0; // 0 = Console, 1 = Assets
}; 