#include "GraphicsManager.hpp"

#include <Engine/EngineUtil.hpp>
#include <fstream>
#include <iostream>

#include "Model.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "OpenGLUtil.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"

ShaderProgram& GraphicsManager::loadShader(const std::string& vShaderFile, const std::string& fShaderFile, const std::string& name)
{
	auto result = shader_map.emplace(std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(vShaderFile, fShaderFile));
	//shader_map.emplace(name, vShaderFile, fShaderFile);
	return result.first->second;
}

ShaderProgram & GraphicsManager::getShader(const std::string & name)
{
	return shader_map.at(name);
}

Texture& GraphicsManager::loadTexture(const std::string& file, const std::string& name)
{
	auto result = texture_map.emplace(std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(file));
	//texture_map.emplace(name, file);
	return result.first->second;
}

Texture & GraphicsManager::getTexture(const std::string & name)
{
	return texture_map.at(name);
}

Mesh & GraphicsManager::createMesh(const std::string& name,  const std::vector<Vertex>& vertices, const std::string& material_name)
{
	auto result = mesh_map.emplace(std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(vertices, material_name));
	//mesh_map.emplace(name, vertices, material_name);
	return result.first->second;
}

Mesh & GraphicsManager::getMesh(const std::string & name)
{
	return mesh_map.at(name);
}

Material& GraphicsManager::createMaterial(const std::string& name, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum,
	const std::string& map_Ka_path, const std::string& map_Kd_path, const std::string& map_Ks_path, const std::string& map_Ns_path,
	const std::string& map_d_path, const std::string& map_bump_path)
{
	auto result = material_map.emplace(std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(Ka, Kd, Ks, Ns, Ni, d, illum, map_Ka_path, map_Kd_path, map_Ks_path, map_Ns_path, map_d_path, map_bump_path));
	//material_map.emplace(name, Ka, Kd, Ks, Ns, Ni, d, illum, map_Ka_path, map_Kd_path, map_Ks_path, map_Ns_path, map_d_path, map_bump_path);
	return result.first->second;
}

Material& GraphicsManager::getMaterial(const std::string& name)
{
	return material_map.at(name);
}

Model& GraphicsManager::createModel(const std::vector<std::string>& mesh_names, const std::string& name)
{
	auto result = model_map.emplace(std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(mesh_names));
	//model_map.emplace(name, mesh_names);
	return result.first->second;
}

Model & GraphicsManager::createModelFromObj(const std::string& file, const std::string & name)
{
	return createModel(loadObj(file), name);
}

Model & GraphicsManager::getModel(const std::string & name)
{
	return model_map.at(name);
}

void GraphicsManager::Clear()
{
	// (properly) delete all shaders
	for( auto & iter : shader_map )
		OpenGlUtil::deleteShaderProgram(iter.second.ID);
	// (properly) delete all textures
	for( auto & iter : texture_map )
	{
		iter.second.deleteTexture();
	}
	shader_map.clear();
	texture_map.clear();
	mesh_map.clear();
	model_map.clear();
	material_map.clear();
}

std::vector<std::string> GraphicsManager::loadObj(const std::string& file)
{
	auto test = engine_util::buildPath(file);
	std::ifstream obj_file(engine_util::buildPath(file));
	if( ! obj_file.is_open() )
	{
		std::cerr << "Failed to open file: " << file << std::endl;
		return {};
	}

	std::vector<std::string> mesh_names;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vertex> vertices;
	std::string currentMaterial = "Default";

	std::string line, currentMeshName;
	bool firstObject = true;

	temp_vertices.reserve(10000); // Adjust based on expected OBJ size
	temp_uvs.reserve(5000);
	temp_normals.reserve(5000);
	vertexIndices.reserve(30000);
	uvIndices.reserve(30000);
	normalIndices.reserve(30000);
	vertices.reserve(30000);
	mesh_names.reserve(1000);

	auto process_mesh = [&]()
		{
			vertices.reserve(vertexIndices.size());

			for (size_t i = 0; i < vertexIndices.size(); i++)
			{
				Vertex v;
				v.position = temp_vertices[vertexIndices[i]];
				v.texture_coordinates = temp_uvs[uvIndices[i]];
				v.normal = temp_normals[normalIndices[i]];
				vertices.emplace_back(v);
			}

			createMesh(currentMeshName, vertices, currentMaterial);
			mesh_names.emplace_back(currentMeshName);
			vertices.clear();
			vertexIndices.clear();
			uvIndices.clear();
			normalIndices.clear();
		};

	while (std::getline(obj_file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		char prefix[16];
		if (sscanf(line.c_str(), "%15s", prefix) != 1)
			continue;

		if (strcmp(prefix, "o") == 0)
		{
			if (!firstObject)
				process_mesh();
			firstObject = false;
			sscanf(line.c_str(), "o %s", &currentMeshName[0]);
			size_t space = line.find(' ');
			if (space != std::string::npos)
				currentMeshName = line.substr(space + 1);
		}
		else if (strcmp(prefix, "v") == 0)
		{
			glm::vec3 vertex;
			sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.emplace_back(vertex);
		}
		else if (strcmp(prefix, "vt") == 0)
		{
			glm::vec2 uv;
			sscanf(line.c_str(), "vt %f %f", &uv.x, &uv.y);
			temp_uvs.emplace_back(uv);
		}
		else if (strcmp(prefix, "vn") == 0)
		{
			glm::vec3 normal;
			sscanf(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
			temp_normals.emplace_back(normal);
		}
		else if (strcmp(prefix, "f") == 0)
		{
			unsigned int vIdx[3], uvIdx[3], nIdx[3];
			sscanf(line.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u",
				&vIdx[0], &uvIdx[0], &nIdx[0],
				&vIdx[1], &uvIdx[1], &nIdx[1],
				&vIdx[2], &uvIdx[2], &nIdx[2]);

			for (int i = 0; i < 3; ++i)
			{
				vertexIndices.emplace_back(vIdx[i] - 1);
				uvIndices.emplace_back(uvIdx[i] - 1);
				normalIndices.emplace_back(nIdx[i] - 1);
			}
		}
		else if (strcmp(prefix, "usemtl") == 0)
		{
			size_t space = line.find(' ');
			if (space != std::string::npos)
				currentMaterial = line.substr(space + 1);
		}
		else if (strcmp(prefix, "mtllib") == 0)
		{
			size_t space = line.find(' ');
			if (space != std::string::npos)
			{
				std::string mtl_file = line.substr(space + 1);
				loadMtl(mtl_file.c_str());
			}
		}
	}

	if (!currentMeshName.empty())
		process_mesh();

	return mesh_names;
}



std::vector<std::string> GraphicsManager::loadMtl(const std::string& file)
{

	std::ifstream mtl_file(engine_util::buildPath(file));
	if( ! mtl_file.is_open() )
	{
		std::cerr << "Failed to open file: " << file << std::endl;
		return {};
	}

	std::vector<std::string> material_names;
	std::string line;
	std::string currentMaterialName;
	std::string map_Ka_path, map_Kd_path, map_Ks_path, map_Ns_path, map_d_path, map_bump_path;
	float Ns = 0.0f;
	glm::vec3 Ka(0.0f), Kd(0.0f), Ks(0.0f);
	float Ni = 0.0f;
	float d = 0.0f;
	int illum = 0;

	while (std::getline(mtl_file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		char prefix[16];
		if (sscanf(line.c_str(), "%15s", prefix) != 1)
			continue;

		if (strcmp(prefix, "newmtl") == 0)
		{
			if (!currentMaterialName.empty())
			{
				createMaterial(currentMaterialName, Ka, Kd, Ks, Ns, Ni, d, illum,
					map_Ka_path, map_Kd_path, map_Ks_path, map_Ns_path,
					map_d_path, map_bump_path);
				material_names.emplace_back(currentMaterialName);
			}

			char name[256];
			sscanf(line.c_str(), "newmtl %255s", name);
			currentMaterialName = std::string(name);

			// Reset properties
			map_Ka_path.clear();
			map_Kd_path.clear();
			map_Ks_path.clear();
			map_Ns_path.clear();
			map_d_path.clear();
			map_bump_path.clear();
			Ns = 0.0f;
			Ka = glm::vec3(0.0f);
			Kd = glm::vec3(0.0f);
			Ks = glm::vec3(0.0f);
			Ni = 0.0f;
			d = 0.0f;
			illum = 0;
		}
		else if (strcmp(prefix, "map_Ka") == 0)
		{
			char path[256];
			sscanf(line.c_str(), "map_Ka %255s", path);
			map_Ka_path = std::string(path);
		}
		else if (strcmp(prefix, "map_Kd") == 0)
		{
			char path[256];
			sscanf(line.c_str(), "map_Kd %255s", path);
			map_Kd_path = std::string(path);
		}
		else if (strcmp(prefix, "map_Ks") == 0)
		{
			char path[256];
			sscanf(line.c_str(), "map_Ks %255s", path);
			map_Ks_path = std::string(path);
		}
		else if (strcmp(prefix, "map_Ns") == 0)
		{
			char path[256];
			sscanf(line.c_str(), "map_Ns %255s", path);
			map_Ns_path = std::string(path);
		}
		else if (strcmp(prefix, "map_d") == 0)
		{
			char path[256];
			sscanf(line.c_str(), "map_d %255s", path);
			map_d_path = std::string(path);
		}
		else if (strcmp(prefix, "map_bump") == 0)
		{
			char path[256];
			sscanf(line.c_str(), "map_bump %255s", path);
			map_bump_path = std::string(path);
		}
		else if (strcmp(prefix, "Ns") == 0)
		{
			sscanf(line.c_str(), "Ns %f", &Ns);
		}
		else if (strcmp(prefix, "Ka") == 0)
		{
			sscanf(line.c_str(), "Ka %f %f %f", &Ka.x, &Ka.y, &Ka.z);
		}
		else if (strcmp(prefix, "Kd") == 0)
		{
			sscanf(line.c_str(), "Kd %f %f %f", &Kd.x, &Kd.y, &Kd.z);
		}
		else if (strcmp(prefix, "Ks") == 0)
		{
			sscanf(line.c_str(), "Ks %f %f %f", &Ks.x, &Ks.y, &Ks.z);
		}
		else if (strcmp(prefix, "Ni") == 0)
		{
			sscanf(line.c_str(), "Ni %f", &Ni);
		}
		else if (strcmp(prefix, "d") == 0)
		{
			sscanf(line.c_str(), "d %f", &d);
		}
		else if (strcmp(prefix, "illum") == 0)
		{
			sscanf(line.c_str(), "illum %d", &illum);
		}
	}

	if (!currentMaterialName.empty())
	{
		createMaterial(currentMaterialName, Ka, Kd, Ks, Ns, Ni, d, illum,
			map_Ka_path, map_Kd_path, map_Ks_path, map_Ns_path,
			map_d_path, map_bump_path);
		material_names.emplace_back(currentMaterialName);
	}

	return material_names;
}