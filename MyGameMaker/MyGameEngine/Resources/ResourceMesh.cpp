#include "ResourceMesh.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Log.h"

bool ResourceMesh::SaveToLibrary() 
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(originalFile,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG(LogType::LOG_WARNING, ("Failed to load mesh: " + originalFile).c_str());
        return false;
    }

    // Process the first mesh only for now
    if (scene->mNumMeshes > 0) {
        aiMesh* mesh = scene->mMeshes[0];
        
        // Get vertices
        vertices.clear();
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Position
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);
            
            // Normals
            if (mesh->HasNormals()) {
                vertices.push_back(mesh->mNormals[i].x);
                vertices.push_back(mesh->mNormals[i].y);
                vertices.push_back(mesh->mNormals[i].z);
            }
            
            // UV coordinates
            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
        }

        // Get indices
        indices.clear();
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Make sure the library path has the correct extension
        if (libraryFile.find(".mymesh") == std::string::npos) {
            libraryFile += ".mymesh";
        }

        // Save to custom format
        std::ofstream file(libraryFile, std::ios::binary);
        if (!file.is_open()) {
            LOG(LogType::LOG_WARNING, ("Failed to create library file: " + libraryFile).c_str());
            return false;
        }

        // Write mesh info header
        uint32_t vertexCount = vertices.size();
        uint32_t indexCount = indices.size();
        uint32_t vertexStride = (mesh->HasNormals() ? 6 : 3) + (mesh->HasTextureCoords(0) ? 2 : 0);
        
        file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
        file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
        file.write(reinterpret_cast<const char*>(&vertexStride), sizeof(vertexStride));

        // Write mesh data
        file.write(reinterpret_cast<const char*>(vertices.data()), vertices.size() * sizeof(float));
        file.write(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(unsigned int));

        file.close();
        LOG(LogType::LOG_INFO, ("Mesh processed and saved to library: " + libraryFile).c_str());
        return true;
    }

    return false;
}

bool ResourceMesh::LoadToMemory() 
{
    // Make sure we're looking for the right extension
    if (libraryFile.find(".mymesh") == std::string::npos) {
        libraryFile += ".mymesh";
    }

    std::ifstream file(libraryFile, std::ios::binary);
    if (!file.is_open()) {
        LOG(LogType::LOG_WARNING, ("Failed to open library file: " + libraryFile).c_str());
        return false;
    }

    try {
        // Read mesh info header
        uint32_t vertexCount, indexCount, vertexStride;
        file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        file.read(reinterpret_cast<char*>(&vertexStride), sizeof(vertexStride));

        // Read mesh data
        vertices.resize(vertexCount);
        indices.resize(indexCount);

        file.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(float));
        file.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int));

        file.close();
        loaded = true;
        LOG(LogType::LOG_INFO, ("Mesh loaded from library: " + libraryFile).c_str());
        return true;
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_WARNING, ("Error loading mesh: " + std::string(e.what())).c_str());
        return false;
    }
} 