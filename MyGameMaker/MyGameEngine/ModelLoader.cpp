#include "ModelLoader.h"

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>
#include <filesystem>
#include <iostream>

void ModelLoader::load(const std::string& filename, std::vector<std::shared_ptr<Model>>& models) const
{
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (nullptr == scene) {
		std::cout << importer.GetErrorString() << std::endl;
	}
	else {
		models.resize(scene->mNumMeshes);

		std::vector<std::shared_ptr<ModelData>> modelsData;
		modelsData.resize(scene->mNumMeshes);

		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];
			modelsData[i] = std::make_shared<ModelData>();
			models[i] = std::make_shared<Model>();
			models[i]->SetMeshName(mesh->mName.C_Str());

			// Obtener el material del mesh
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// Obtener la textura difusa (si existe)
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString path;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
					std::string texturePath = path.C_Str();
					

					// Extraer solo el nombre del archivo
					std::filesystem::path filePath(texturePath);
					std::string fileName = filePath.filename().string();

					// Construir la nueva ruta con un directorio específico
					std::string newDirectory = "Assets/Textures/";
					std::string newPath = newDirectory + fileName;

					std::cout << "Textura difusa: " << newPath << std::endl;
					// Aquí puedes cargar la textura usando tu método preferido
					models[i]->AddTexture(newPath);
				}
			}

			for (unsigned int j = 0; j < mesh->mNumVertices; j++) {

				// Coordenadas de los vértices
				aiVector3D vertex = mesh->mVertices[j];
				vec3 aux = vec3(vertex.x, vertex.y, vertex.z);
				modelsData[i]->vertexData.push_back(aux);

				// Coordenadas UV (si existen)
				if (mesh->mTextureCoords[0]) {  // Comprueba si hay UVs
					aiVector3D uv = mesh->mTextureCoords[0][j];
					aux.x = uv.x;  // Solo X y Y
					aux.y = 1.0f - uv.y;
				}
				modelsData[i]->vertex_texCoords.push_back(aux);

				if (mesh->HasNormals()) {  // Verifica si hay normales
					aiVector3D normal = mesh->mNormals[j];
					vec3 auxNormal(normal.x, normal.y, normal.z);
					modelsData[i]->vertex_normals.push_back(auxNormal);
				}

				if (mesh->HasVertexColors(0)) {  // Verifica si hay colores
					aiColor4D color = mesh->mColors[0][j];
					vec3 auxColor(color.r, color.g, color.b);
					modelsData[i]->vertex_colors.push_back(auxColor);
				}

			}

			for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
				aiFace face = mesh->mFaces[j];
				for (unsigned int k = 0; k < face.mNumIndices; k++) {
					modelsData[i]->indexData.push_back(face.mIndices[k]);
				}
			}

			models[i]->SetModelData(*modelsData[i]);

			models[i]->SaveModelData(models[i]->GetMeshName() + ".model");

		}

	}
}

void ModelLoader::load(Shapes shape, std::shared_ptr<Model>& model)
{

	std::shared_ptr<ModelData> modelData = std::make_shared<ModelData>();
	model = std::make_shared<Model>();

	const float PI = 3.14159265358979323846f;
	const int slices = 36; // number of radial slices
	const int stacks = 18; // number of stacks (height divisions)
	const float radius = 1.0f; // radius of the sphere
	const float height = 2.0f; // height of the cylinder

	switch (shape)
	{
	case Shapes::CUBE:
		model->SetMeshName("Cube");

		modelData->vertexData = {
			vec3(-1,-1,-1),
			vec3(1,-1,-1), 
			vec3(1,1,-1),  
			vec3(-1,1,-1), 
			vec3(-1,-1,1), 
			vec3(1,-1,1),  
			vec3(1,1,1),   
			vec3(-1,1,1)  
		};

		modelData->vertex_colors = {
			vec3(0.8,0,0),
			vec3(0.0,1,0),
			vec3(0.0,0,1),
			vec3(1,1,1),
			vec3(1,0,1),
			vec3(0.5,1,0),
			vec3(0.8,1,0.5),
			vec3(0.8,0.3,0.6)
		};

		modelData->indexData = {
			0, 1, 2, 0, 2, 3,
			1, 5, 6, 1, 6, 2,
			5, 4, 7, 5, 7, 6,
			4, 0, 3, 4, 3, 7,
			3, 2, 6, 3, 6, 7,
			4, 5, 1, 4, 1, 0
		};
		break;
	
	case Shapes::PLANE:
		model->SetMeshName("Plane");

		modelData->vertexData = {
			vec3(-1,0,-1),
			vec3(1,0,-1),
			vec3(1,0,1),
			vec3(-1,0,1)
		};

		modelData->vertex_colors = {
			vec3(0.8,0,0),
			vec3(0.0,1,0),
			vec3(0.0,0,1),
			vec3(1,1,1)
		};

		modelData->indexData = {
			0, 1, 2, 0, 2, 3
		};
		break;
	
	case Shapes::SPHERE:
		model->SetMeshName("Sphere");

		for (int i = 0; i <= stacks; ++i)
		{
			float theta = i * PI / stacks; // latitude angle
			for (int j = 0; j <= slices; ++j)
			{
				float phi = j * 2.0f * PI / slices; // longitude angle
				float x = radius * sin(theta) * cos(phi);
				float y = radius * cos(theta);
				float z = radius * sin(theta) * sin(phi);

				modelData->vertexData.push_back(vec3(x, y, z));
				modelData->vertex_colors.push_back(vec3(1.0f, 0.0f, 0.0f)); // red color
			}
		}

		// Generate indices for the sphere
		for (int i = 0; i < stacks; ++i)
		{
			for (int j = 0; j < slices; ++j)
			{
				int first = i * (slices + 1) + j;
				int second = first + slices + 1;
				if (i != 0)
				{
					modelData->indexData.push_back(first);
					modelData->indexData.push_back(second);
					modelData->indexData.push_back(first + 1);
				}
				if (i != (stacks - 1))
				{
					modelData->indexData.push_back(second);
					modelData->indexData.push_back(second + 1);
					modelData->indexData.push_back(first + 1);
				}
			}
		}
		break;

	case Shapes::CYLINDER:
		model->SetMeshName("Cylinder");

		for (int i = 0; i <= stacks; ++i)
		{
			float y = (i / (float)stacks) * height - height / 2.0f;
			for (int j = 0; j < slices; ++j)
			{
				float theta = j * 2.0f * PI / slices;
				float x = radius * cos(theta);
				float z = radius * sin(theta);
				modelData->vertexData.push_back(vec3(x, y, z));
				modelData->vertex_colors.push_back(vec3(0.0f, 1.0f, 0.0f)); // green color
			}
		}

		// Create top and bottom caps of the cylinder
		for (int j = 0; j < slices; ++j)
		{
			float theta = j * 2.0f * PI / slices;
			float x = radius * cos(theta);
			float z = radius * sin(theta);
			modelData->vertexData.push_back(vec3(x, height / 2.0f, z)); // top cap
			modelData->vertexData.push_back(vec3(x, -height / 2.0f, z)); // bottom cap
			modelData->vertex_colors.push_back(vec3(0.0f, 1.0f, 0.0f)); // green color
		}

		// Create the cylinder body indices
		for (int i = 0; i < stacks; ++i)
		{
			for (int j = 0; j < slices; ++j)
			{
				int first = i * slices + j;
				int second = (i + 1) * slices + j;
				int next = (j + 1) % slices;
				int third = (i + 1) * slices + next;
				modelData->indexData.push_back(first);
				modelData->indexData.push_back(second);
				modelData->indexData.push_back(third);
				modelData->indexData.push_back(first);
				modelData->indexData.push_back(third);
				modelData->indexData.push_back(next);
			}
		}

		// Add top and bottom cap indices
		for (int j = 0; j < slices; ++j)
		{
			int top_center = modelData->vertexData.size() - 2 * slices;
			int bottom_center = modelData->vertexData.size() - slices;
			int first = j;
			int second = (j + 1) % slices;
			modelData->indexData.push_back(top_center);
			modelData->indexData.push_back(first);
			modelData->indexData.push_back(second);
			modelData->indexData.push_back(bottom_center);
			modelData->indexData.push_back(second + slices);
			modelData->indexData.push_back(first + slices);
		}
		break;

	case Shapes::CONE:
		model->SetMeshName("Cone");

		// Vertices for cone
		modelData->vertexData.push_back(vec3(0.0f, height / 2.0f, 0.0f)); // top (apex)
		modelData->vertex_colors.push_back(vec3(1.0f, 0.0f, 0.0f)); // red color

		for (int i = 0; i < slices; ++i)
		{
			float theta = i * 2.0f * PI / slices;
			float x = radius * cos(theta);
			float z = radius * sin(theta);
			modelData->vertexData.push_back(vec3(x, -height / 2.0f, z)); // base vertices
			modelData->vertex_colors.push_back(vec3(0.0f, 1.0f, 0.0f)); // green color
		}

		// Cone body indices
		for (int i = 0; i < slices; ++i)
		{
			int first = i + 1;
			int second = (i + 1) % slices + 1;
			modelData->indexData.push_back(0); // apex
			modelData->indexData.push_back(first);
			modelData->indexData.push_back(second);
		}

		// Base indices (not required but could be included for completeness)
		for (int i = 0; i < slices; ++i)
		{
			int first = i + 1;
			int second = (i + 1) % slices + 1;
			modelData->indexData.push_back(first);
			modelData->indexData.push_back(second);
			modelData->indexData.push_back(slices + 1);
		}
		break;

	case Shapes::TORUS:
		model->SetMeshName("Torus");

		// Torus parameters
		float R = 1.0f;  // major radius (distance from the center of the torus to the center of the tube)
		float r = 0.4f;  // minor radius (radius of the tube itself)
		int numTorusSlices = 36; // Number of slices around the tube
		int numTorusStacks = 18; // Number of stacks (divisions around the major radius)

		// Generate the vertices for the torus
		for (int i = 0; i < numTorusStacks; ++i)
		{
			float theta = i * 2.0f * PI / numTorusStacks; // Angle around the major radius
			for (int j = 0; j < numTorusSlices; ++j)
			{
				float phi = j * 2.0f * PI / numTorusSlices; // Angle around the minor radius

				// Parametric equations for a torus:
				float x = (R + r * cos(phi)) * cos(theta);  // x = (major radius + minor radius * cos(phi)) * cos(theta)
				float y = (R + r * cos(phi)) * sin(theta);  // y = (major radius + minor radius * cos(phi)) * sin(theta)
				float z = r * sin(phi);                     // z = minor radius * sin(phi)

				modelData->vertexData.push_back(vec3(x, y, z)); // Store the vertex
				modelData->vertex_colors.push_back(vec3(1.0f, 0.5f, 0.0f)); // Color the vertices (orange for example)
			}
		}

		// Generate the indices for the torus
		for (int i = 0; i < numTorusStacks; ++i)
		{
			for (int j = 0; j < numTorusSlices; ++j)
			{
				// Vertices around the current stack
				int first = i * numTorusSlices + j;
				int second = ((i + 1) % numTorusStacks) * numTorusSlices + j;
				int next = (j + 1) % numTorusSlices;
				int third = ((i + 1) % numTorusStacks) * numTorusSlices + next;

				// First triangle
				modelData->indexData.push_back(first);
				modelData->indexData.push_back(second);
				modelData->indexData.push_back(third);

				// Second triangle
				modelData->indexData.push_back(first);
				modelData->indexData.push_back(third);
				modelData->indexData.push_back(first + 1);
			}
		}
		break;
	}

	model->SetModelData(*modelData);

	model->SaveModelData(model->GetMeshName() + ".model");
}
