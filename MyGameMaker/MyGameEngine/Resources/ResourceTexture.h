#pragma once
#include "../ResourceManager.h"
#include <vector>

class ResourceTexture : public Resource {
public:
    ResourceTexture(const std::string& uuid) : Resource(Type::TEXTURE, uuid) {}
    
    bool LoadToMemory() override;
    bool SaveToLibrary() override;
    
    // Texture data
    unsigned int textureID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<unsigned char> data;
}; 