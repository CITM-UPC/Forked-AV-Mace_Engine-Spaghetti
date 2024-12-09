#include "Model.h"
#include <fstream>
#include <filesystem>
#include <iostream>

void Model::SaveModelData(const std::string& filepath) {
   std::filesystem::path folderPath = std::filesystem::current_path() / "SaveFiles";

    // Verificar si la carpeta existe y crearla si no
    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directory(folderPath);
    }
    std::string filename = (folderPath / filepath).string();

    std::ofstream file(filename, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Write mesh name
    size_t nameLength = meshName.size();
    file.write(reinterpret_cast<const char*>(&nameLength), sizeof(size_t));
    file.write(meshName.c_str(), nameLength);

    // Write model data
    size_t vertexCount = modelData.vertexData.size();
    file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(modelData.vertexData.data()), vertexCount * sizeof(vec3));

    size_t indexCount = modelData.indexData.size();
    file.write(reinterpret_cast<const char*>(&indexCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(modelData.indexData.data()), indexCount * sizeof(unsigned int));

    size_t texCoordCount = modelData.vertex_texCoords.size();
    file.write(reinterpret_cast<const char*>(&texCoordCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(modelData.vertex_texCoords.data()), texCoordCount * sizeof(vec2));

    size_t normalCount = modelData.vertex_normals.size();
    file.write(reinterpret_cast<const char*>(&normalCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(modelData.vertex_normals.data()), normalCount * sizeof(vec3));

    size_t colorCount = modelData.vertex_colors.size();
    file.write(reinterpret_cast<const char*>(&colorCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(modelData.vertex_colors.data()), colorCount * sizeof(vec3));

    file.close();
}
void Model::AddTexture(const std::string& texturePath) {
    textures.push_back(texturePath);
}