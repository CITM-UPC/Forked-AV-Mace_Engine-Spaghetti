#include "PanelAssets.h"
#include <imgui.h>
#include <filesystem>
#include "MyGameEngine/Engine.h"
#include "MyGameEngine/Log.h"
#include "MyGameEngine/ResourceManager.h"

namespace fs = std::filesystem;

PanelAssets::PanelAssets(std::string name) : Panel(name, WINDOW_WIDTH, 200)
{
    SwitchState();
    RefreshAssetList();
}

PanelAssets::~PanelAssets() {}

bool PanelAssets::Draw()
{
    // This is now just a stub since we're drawing in the Console panel
    return true;
}

void PanelAssets::DrawContents()
{
    // Current path display
    ImGui::Text("Current Path: %s", currentPath.c_str());
    ImGui::Separator();

    // Handle file/folder drops into the window
    HandleDragAndDrop();

    // Draw directory contents in a child window to enable proper scrolling
    ImGui::BeginChild("AssetsView", ImVec2(0, 0), false);
    
    for (const auto& entry : currentAssets)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (entry.isDirectory) {
            flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        }

        std::string nodeId = entry.name + "##" + entry.path;
        bool nodeOpen = ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", entry.name.c_str());

        // Handle clicking on items
        if (ImGui::IsItemClicked() && entry.isDirectory) {
            if (entry.name == "..") {
                currentPath = entry.path;
            } else {
                currentPath = entry.path;
            }
            RefreshAssetList();
        }

        // Context menu for deletion
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                showDeleteConfirmation = true;
                assetToDelete = entry.path;
            }
            ImGui::EndPopup();
        }

        if (nodeOpen && entry.isDirectory) {
            ImGui::TreePop();
        }
    }

    ImGui::EndChild();

    // Delete confirmation modal
    if (showDeleteConfirmation) {
        ImGui::OpenPopup("Delete Asset?");
    }

    if (ImGui::BeginPopupModal("Delete Asset?", &showDeleteConfirmation)) {
        ImGui::Text("Are you sure you want to delete this asset?");
        ImGui::Text("This action cannot be undone.");
        
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
            DeleteAsset(assetToDelete);
            showDeleteConfirmation = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120, 0))) {
            showDeleteConfirmation = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void PanelAssets::RefreshAssetList()
{
    currentAssets.clear();
    
    // Add parent directory entry if we're not at root
    if (currentPath != "Assets") {
        AssetEntry parentDir;
        parentDir.name = "..";
        parentDir.path = fs::path(currentPath).parent_path().string();
        parentDir.isDirectory = true;
        currentAssets.push_back(parentDir);
    }

    try {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            AssetEntry assetEntry;
            assetEntry.name = entry.path().filename().string();
            assetEntry.path = entry.path().string();
            assetEntry.isDirectory = entry.is_directory();
            currentAssets.push_back(assetEntry);
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, ("Failed to read directory: " + std::string(e.what())).c_str());
    }
}

void PanelAssets::HandleDragAndDrop()
{
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXTERNAL_FILE")) {
        char* path = (char*)payload->Data;
        try {
            fs::path sourcePath(path);
            fs::path destPath = currentPath / sourcePath.filename();
            
            if (fs::exists(destPath)) {
                LOG(LogType::LOG_WARNING, ("File already exists: " + destPath.string()).c_str());
            }
            else {
                // Copy to Assets folder
                fs::copy(sourcePath, destPath, fs::copy_options::recursive);
                
                // Import through ResourceManager
                auto resource = ResourceManager::Instance().ImportFile(destPath.string());
                if (resource) {
                    LOG(LogType::LOG_INFO, ("Imported and processed: " + destPath.string()).c_str());
                }
                
                RefreshAssetList();
            }
        }
        catch (const std::exception& e) {
            LOG(LogType::LOG_WARNING, ("Failed to import file: " + std::string(e.what())).c_str());
        }
    }
}

void PanelAssets::DeleteAsset(const std::string& path)
{
    try {
        if (fs::remove_all(path) > 0) {
            LOG(LogType::LOG_INFO, ("Deleted: " + path).c_str());
            
            // Also remove the corresponding .meta file if it exists
            std::string metaPath = path + ".meta";
            if (fs::exists(metaPath)) {
                fs::remove(metaPath);
            }
            
            RefreshAssetList();
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, ("Failed to delete asset: " + std::string(e.what())).c_str());
    }
}