#include "ResourceManager.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"
#include <filesystem>
#include <fstream>
#include <random>
#include "Log.h"

namespace fs = std::filesystem;

std::shared_ptr<Resource> ResourceManager::ImportFile(const std::string& assetPath) 
{
    // Generate a UUID for the new resource
    std::string uuid = GenerateUUID();
    
    // Determine resource type from file extension
    Resource::Type type = GetResourceTypeFromExtension(assetPath);
    
    // Create appropriate resource based on type
    std::shared_ptr<Resource> resource = nullptr;
    
    switch (type) {
        case Resource::Type::MESH:
            resource = std::make_shared<ResourceMesh>(uuid);
            break;
        case Resource::Type::TEXTURE:
            resource = std::make_shared<ResourceTexture>(uuid);
            break;
        default:
            LOG(LogType::LOG_WARNING, "Unsupported file type for import");
            return nullptr;
    }
    
    // Set file paths
    resource->originalFile = assetPath;
    resource->libraryFile = "Library/" + uuid;
    
    // Add extension based on type
    if (type == Resource::Type::MESH) resource->libraryFile += ".mymesh";
    else if (type == Resource::Type::TEXTURE) resource->libraryFile += ".mytex";
    
    // Generate meta file
    GenerateMetaFile(assetPath, uuid);
    
    // Process and save the resource to the library
    if (!resource->SaveToLibrary()) {
        LOG(LogType::LOG_WARNING, "Failed to save resource to library");
        return nullptr;
    }
    
    // Store in resource map
    resources[uuid] = resource;
    
    return resource;
}

std::string ResourceManager::GenerateUUID() 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* digits = "0123456789abcdef";

    std::string uuid;
    for (int i = 0; i < 32; ++i) {
        uuid += digits[dis(gen)];
    }
    return uuid;
}

void ResourceManager::GenerateMetaFile(const std::string& assetPath, const std::string& uuid) 
{
    std::string metaPath = assetPath + ".meta";
    std::ofstream metaFile(metaPath);
    
    if (metaFile.is_open()) {
        metaFile << "uuid: " << uuid << "\n";
        metaFile << "type: " << static_cast<int>(GetResourceTypeFromExtension(assetPath)) << "\n";
        metaFile.close();
        LOG(LogType::LOG_INFO, ("Meta file created: " + metaPath).c_str());
    } else {
        LOG(LogType::LOG_WARNING, ("Failed to create meta file: " + metaPath).c_str());
    }
}

Resource::Type ResourceManager::GetResourceTypeFromExtension(const std::string& path) 
{
    std::string ext = fs::path(path).extension().string();
    
    if (ext == ".fbx" || ext == ".obj") return Resource::Type::MESH;
    if (ext == ".png" || ext == ".jpg" || ext == ".tga") return Resource::Type::TEXTURE;
    if (ext == ".mat") return Resource::Type::MATERIAL;
    if (ext == ".glsl" || ext == ".vert" || ext == ".frag") return Resource::Type::SHADER;
    
    return Resource::Type::MESH; // Default type
}

std::shared_ptr<Resource> ResourceManager::GetResource(const std::string& uuid) 
{
    auto it = resources.find(uuid);
    if (it != resources.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::UnloadUnusedResources() 
{
    // TODO: Implement reference counting and resource unloading
} 