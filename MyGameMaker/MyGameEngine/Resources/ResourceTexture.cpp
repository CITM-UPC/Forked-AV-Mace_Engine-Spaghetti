#include "ResourceTexture.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <fstream>
#include <string>
#include "../Log.h"
#include <GL/glew.h>


bool ResourceTexture::LoadToMemory() 
{
    // Load from library file
    std::ifstream file(libraryFile, std::ios::binary);
    if (!file.is_open()) {
        LOG(LogType::LOG_WARNING, ("Failed to open library file: " + libraryFile).c_str());
        return false;
    }

    // Read header
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    file.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    // Read pixel data
    size_t dataSize = width * height * channels;
    data.resize(dataSize);
    file.read(reinterpret_cast<char*>(data.data()), dataSize);

    // Generate OpenGL texture
    if (textureID == 0) {
        glGenTextures(1, &textureID);
    }
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                 channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    loaded = true;
    return true;
}

bool ResourceTexture::SaveToLibrary() 
{
    // Load original image using DevIL
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);
    
    // Use regular string for DevIL on Windows
    if (!ilLoadImage((ILstring)originalFile.c_str())) {
        LOG(LogType::LOG_WARNING, ("Failed to load texture: " + originalFile).c_str());
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Get image data
    width = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);
    channels = ilGetInteger(IL_IMAGE_CHANNELS);
    
    // Convert to RGBA if needed
    if (channels < 3) {
        ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
        channels = 3;
    }

    // Get pixel data
    data.resize(width * height * channels);
    ilCopyPixels(0, 0, 0, width, height, 1, 
                 channels == 4 ? IL_RGBA : GL_RGB, IL_UNSIGNED_BYTE, data.data());

    // Save to library file
    std::ofstream file(libraryFile, std::ios::binary);
    if (!file.is_open()) {
        LOG(LogType::LOG_WARNING, ("Failed to create library file: " + libraryFile).c_str());
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Write header
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&channels), sizeof(channels));

    // Write pixel data
    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    ilDeleteImages(1, &imageID);
    LOG(LogType::LOG_INFO, ("Texture processed and saved to library: " + libraryFile).c_str());
    return true;
} 