#include "PanelAssets.h"
#include <imgui.h>
#include <filesystem>
#include <fstream>
#include "MyGameEngine/Engine.h"
#include "MyGameEngine/MyWindow.h"
#include "MyGameEngine/Log.h"
#include "MyGameEngine/ResourceManager.h"

namespace fs = std::filesystem;

PanelAssets::PanelAssets(std::string name) : Panel(name, WINDOW_WIDTH, 200)
{
    SwitchState();
    
    // Create Assets directory if it doesn't exist
    if (!fs::exists("Assets")) {
        fs::create_directory("Assets");
    }
    
    // Initial sync with Library
    SyncLibraryWithAssets();
    RefreshAssetList();
}

bool PanelAssets::Draw()
{
    // Don't create a new window, just draw contents if this tab is active
    if (isActiveTab) {
        DrawContents();
    }
    return true;
}

void PanelAssets::DrawContents()
{
    static float lastRefreshTime = 0.0f;
    const float REFRESH_COOLDOWN = 0.5f; // Half second cooldown

    // Current path display
    ImGui::Text("Current Path: %s", currentPath.c_str());
    ImGui::SameLine();
    
    // Add refresh button with cooldown
    float currentTime = ImGui::GetTime();
    bool refreshEnabled = (currentTime - lastRefreshTime) >= REFRESH_COOLDOWN;
    
    if (!refreshEnabled) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }
    
    if (ImGui::Button("Refresh") && refreshEnabled) {
        SyncLibraryWithAssets();
        RefreshAssetList();
        lastRefreshTime = currentTime;
    }
    
    if (!refreshEnabled) {
        ImGui::PopStyleVar();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Please wait %.1f seconds before refreshing again", 
                REFRESH_COOLDOWN - (currentTime - lastRefreshTime));
        }
    }
    
    ImGui::Separator();

    // Handle file/folder drops into the window
    HandleDragAndDrop();

    // Draw directory contents in a child window to enable proper scrolling
    ImGui::BeginChild("AssetsView", ImVec2(0, 0), false);
    
    for (const auto& entry : currentAssets)
    {
        if (entry.isMetaFile) continue; // Skip .meta files in display

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

void PanelAssets::SyncLibraryWithAssets()
{
    static std::unordered_map<std::string, std::filesystem::file_time_type> lastModifiedTimes;
    static bool firstRun = true;
    
    // Create Library if it doesn't exist
    if (!fs::exists("Library")) {
        fs::create_directory("Library");
    }

    // On first run, clear and rebuild Library
    if (firstRun) {
        fs::remove_all("Library");
        fs::create_directory("Library");
        firstRun = false;
    }

    // Process all files in Assets
    try {
        for (const auto& entry : fs::recursive_directory_iterator("Assets")) {
            if (!entry.is_regular_file() || entry.path().extension() == ".meta") {
                continue;
            }

            std::string path = entry.path().string();
            auto lastWriteTime = fs::last_write_time(entry.path());

            // Check if file needs processing
            auto it = lastModifiedTimes.find(path);
            if (it != lastModifiedTimes.end() && it->second == lastWriteTime) {
                continue;  // File hasn't changed
            }

            // Check if corresponding library file exists
            std::string uuid;
            std::string metaPath = path + ".meta";
            if (fs::exists(metaPath)) {
                uuid = GetUUIDFromMetaFile(metaPath);
                if (!uuid.empty()) {
                    std::string libPath = "Library/" + uuid;
                    if (fs::exists(libPath + ".mymesh") || fs::exists(libPath + ".mytex")) {
                        lastModifiedTimes[path] = lastWriteTime;
                        continue;  // Library file exists and is up to date
                    }
                }
            }

            // Import file if needed
            auto resource = ResourceManager::Instance().ImportFile(path);
            if (resource) {
                lastModifiedTimes[path] = lastWriteTime;
                LOG(LogType::LOG_INFO, ("Processed: " + path).c_str());
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG(LogType::LOG_WARNING, ("Filesystem error: " + std::string(e.what())).c_str());
    }
}

void PanelAssets::DeleteAsset(const std::string& path)
{
    try {
        // Delete the asset file
        if (fs::remove(path)) {
            LOG(LogType::LOG_INFO, ("Deleted asset: " + path).c_str());
            
            // Delete corresponding .meta file
            std::string metaPath = path + ".meta";
            if (fs::exists(metaPath)) {
                // Get UUID before deleting meta file
                std::string uuid = GetUUIDFromMetaFile(metaPath);
                fs::remove(metaPath);
                
                // Delete corresponding Library file
                if (!uuid.empty()) {
                    DeleteLibraryFile(uuid);
                }
            }
            
            RefreshAssetList();
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, ("Failed to delete asset: " + std::string(e.what())).c_str());
    }
}

std::string PanelAssets::GetUUIDFromMetaFile(const std::string& metaPath)
{
    std::ifstream metaFile(metaPath);
    std::string line;
    while (std::getline(metaFile, line)) {
        if (line.find("uuid: ") == 0) {
            return line.substr(6); // Return UUID part after "uuid: "
        }
    }
    return "";
}

void PanelAssets::DeleteLibraryFile(const std::string& uuid)
{
    // Try to delete both possible extensions
    fs::remove("Library/" + uuid + ".mymesh");
    fs::remove("Library/" + uuid + ".mytex");
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
            assetEntry.isMetaFile = entry.path().extension() == ".meta";
            currentAssets.push_back(assetEntry);
        }
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, ("Failed to read directory: " + std::string(e.what())).c_str());
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
        ImGui::EndDragDropTarget();
    }
}

PanelAssets::~PanelAssets() {}