#ifndef __PANEL_ASSETS_H__
#define __PANEL_ASSETS_H__
#pragma once

#include "Panel.h"
#include <string>
#include <vector>
#include <filesystem>

struct AssetEntry {
    std::string name;
    std::string path;
    bool isDirectory;
};

class PanelAssets : public Panel
{
public:
    PanelAssets(std::string name);
    ~PanelAssets();

    bool Draw() override;

private:
    void RefreshAssetList();
    void DrawDirectoryContents(const std::filesystem::path& path);
    void HandleDragAndDrop();
    void DeleteAsset(const std::string& path);

    std::string currentPath = "Assets";
    std::vector<AssetEntry> currentAssets;
    bool showDeleteConfirmation = false;
    std::string assetToDelete;
};

#endif // !__PANEL_ASSETS_H__ 