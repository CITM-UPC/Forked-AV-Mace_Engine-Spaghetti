#pragma once

#include <vector>
#include <string>


#include "types.h"

enum class Shapes
{
	EMPTY,
	CUBE,
	PLANE,
	SPHERE,
	CYLINDER,
	CONE,
	TORUS,
	MESH
};

struct ModelData
{
	unsigned int vBPosID = -1, vBNormalsID = -1, vBColorsID = -1, vBTCoordsID = -1, iBID = -1, vA = -1;
	std::vector<vec3> vertexData;
	std::vector<unsigned int> indexData;
	std::vector<vec2> vertex_texCoords;
	std::vector<vec3> vertex_normals;
	std::vector<vec3> vertex_colors;
	

};

class Model
{
public:
	Model() {}
	~Model() {}

	std::string texturepath;
	std::string& GetMeshName() { return meshName; }

	ModelData& GetModelData() { return modelData; }

	void SetMeshName(const std::string& meshName) { this->meshName = meshName; }

	void SetModelData(const ModelData& modelData) { this->modelData = modelData; }

	void SaveModelData(const std::string& filepath);

	std::string GetTexturePath() { return texturepath; }

	void AddTexture(const std::string& texturePath);
private:
	std::string meshName;
	ModelData modelData;
	
};

