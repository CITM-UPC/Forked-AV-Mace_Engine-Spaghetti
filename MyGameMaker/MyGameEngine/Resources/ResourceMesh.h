#pragma once
#include "ResourceManager.h"

class ResourceMesh : public Resource {
public:
    ResourceMesh(const std::string& uuid) : Resource(Type::MESH, uuid) {}
    
    bool LoadToMemory() override;
    bool SaveToLibrary() override;
    
    // Mesh-specific data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    // etc...
}; 