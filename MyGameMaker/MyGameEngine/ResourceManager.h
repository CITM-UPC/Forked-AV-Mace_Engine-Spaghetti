#pragma once
#include <string>
#include <unordered_map>
#include <memory>

class Resource {
public:
    enum class Type {
        TEXTURE,
        MESH,
        MATERIAL,
        SHADER
    };

    Resource(Type type, const std::string& uuid) : type(type), uuid(uuid) {}
    virtual ~Resource() = default;
    
    virtual bool LoadToMemory() = 0;
    virtual bool SaveToLibrary() = 0;

    Type type;
    std::string uuid;
    std::string originalFile;
    std::string libraryFile;
    bool loaded = false;
};

class ResourceManager {
public:
    static ResourceManager& Instance() {
        static ResourceManager instance;
        return instance;
    }

    std::shared_ptr<Resource> ImportFile(const std::string& assetPath);
    std::shared_ptr<Resource> GetResource(const std::string& uuid);
    void UnloadUnusedResources();

private:
    ResourceManager() = default;
    std::string GenerateUUID();
    void GenerateMetaFile(const std::string& assetPath, const std::string& uuid);
    Resource::Type GetResourceTypeFromExtension(const std::string& path);

    std::unordered_map<std::string, std::shared_ptr<Resource>> resources;
}; 