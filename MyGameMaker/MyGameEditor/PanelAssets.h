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
    bool isMetaFile = false;
};

class PanelAssets : public Panel
{
public:
    PanelAssets(std::string name);
    ~PanelAssets();

    bool Draw() override;
    void DrawContents();
    void SyncLibraryWithAssets();

private:
    void RefreshAssetList();
    void HandleDragAndDrop();
    void DeleteAsset(const std::string& path);
    void DeleteLibraryFile(const std::string& assetPath);
    std::string GetUUIDFromMetaFile(const std::string& metaPath);

    std::string currentPath = "Assets";
    std::vector<AssetEntry> currentAssets;
    bool showDeleteConfirmation = false;
    std::string assetToDelete;
};

#endif // !__PANEL_ASSETS_H__ 