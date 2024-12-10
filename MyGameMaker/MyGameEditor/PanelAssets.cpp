#include "PanelAssets.h"
#include <imgui.h>
#include <filesystem>
#include <algorithm>
#include "MyGameEngine/Engine.h"
#include "MyGameEngine/MyWindow.h"
#include "MyGameEngine/Log.h"

namespace fs = std::filesystem;

PanelAssets::PanelAssets(std::string name) : Panel(name, WINDOW_WIDTH * 0.25, WINDOW_HEIGHT - 200)
{
    SwitchState();
    RefreshAssetList();
}

PanelAssets::~PanelAssets() {}

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

        // Sort: directories first, then files, both alphabetically
        std::sort(currentAssets.begin(), currentAssets.end(), 
            [](const AssetEntry& a, const AssetEntry& b) {
                if (a.name == "..") return true;
                if (b.name == "..") return false;
                if (a.isDirectory != b.isDirectory) return a.isDirectory > b.isDirectory;
                return a.name < b.name;
            });
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, "Failed to read directory: " + std::string(e.what()));
    }
}

void PanelAssets::HandleDragAndDrop()
{
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXTERNAL_FILE")) {
            char* path = (char*)payload->Data;
            try {
                fs::path sourcePath(path);
                fs::path destPath = currentPath / sourcePath.filename();
                
                if (fs::exists(destPath)) {
                    LOG(LogType::LOG_WARNING, "File already exists: " + destPath.string());
                }
                else {
                    fs::copy(sourcePath, destPath, fs::copy_options::recursive);
                    LOG(LogType::LOG_INFO, "Imported file: " + destPath.string());
                    RefreshAssetList();
                }
            }
            catch (const std::exception& e) {
                LOG(LogType::LOG_WARNING, "Failed to import file: " + std::string(e.what()));
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void PanelAssets::DeleteAsset(const std::string& path)
{
    try {
        if (fs::remove_all(path) > 0) {
            LOG(LogType::LOG_INFO, "Deleted: " + path);
            
            // Also remove the corresponding .meta file if it exists
            std::string metaPath = path + ".meta";
            if (fs::exists(metaPath)) {
                fs::remove(metaPath);
            }
            
            RefreshAssetList();
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, "Failed to delete asset: " + std::string(e.what()));
    }
}

bool PanelAssets::Draw()
{
    ImGui::SetNextWindowSize(ImVec2(width, Engine::Instance().window->height() - 200));
    ImGui::SetNextWindowPos(ImVec2(Engine::Instance().window->width() - width * 2, 0));

    ImGui::Begin("Assets", &showWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    // Current path display
    ImGui::Text("Current Path: %s", currentPath.c_str());
    ImGui::Separator();

    // Handle file/folder drops into the window
    HandleDragAndDrop();

    // Draw directory contents
    for (const auto& entry : currentAssets) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (entry.isDirectory) {
            flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        }

        bool nodeOpen = ImGui::TreeNodeEx(entry.name.c_str(), flags);

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

    ImGui::End();

    if (!showWindow) {
        SwitchState();
    }

    return true;
} 