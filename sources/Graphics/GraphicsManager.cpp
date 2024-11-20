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

	auto process_mesh = [&]()
	{
		for( size_t i = 0; i < vertexIndices.size(); i++ )
		{
			Vertex v{};
			v.position = temp_vertices[vertexIndices[i]];
			v.texture_coordinates = temp_uvs[uvIndices[i]];
			v.normal = temp_normals[normalIndices[i]];
			vertices.emplace_back(v);
		}
		//create_mesh(currentMeshName, vertices, "Default");

		createMesh(currentMeshName, vertices, currentMaterial);
		mesh_names.emplace_back(currentMeshName);
	};

	while( std::getline(obj_file, line) )
	{
		std::istringstream line_stream(line);
		std::string prefix;
		line_stream >> prefix;

		if( prefix == "o" )
		{
			if( ! firstObject ) process_mesh();
			firstObject = false;
			line_stream >> currentMeshName;
		}
		else if( prefix == "v" )
		{
			glm::vec3 vertex;
			line_stream >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.emplace_back(vertex);
		}
		else if( prefix == "vt" )
		{
			glm::vec2 uv;
			line_stream >> uv.x >> uv.y;
			temp_uvs.emplace_back(uv);
		}
		else if( prefix == "vn" )
		{
			glm::vec3 normal;
			line_stream >> normal.x >> normal.y >> normal.z;
			temp_normals.emplace_back(normal);
		}
		else if( prefix == "f" )
		{
			for( int i = 0; i < 3; i++ )
			{
				unsigned int vertexIndex, uvIndex, normalIndex;
				char slash;
				line_stream >> vertexIndex >> slash >> uvIndex >> slash >> normalIndex;
				vertexIndices.emplace_back(vertexIndex - 1);
				uvIndices.emplace_back(uvIndex - 1);
				normalIndices.emplace_back(normalIndex - 1);
			}
		}
		else if( prefix == "usemtl" )
		{
			line_stream >> currentMaterial;
		}
		else if (prefix == "mtllib")
		{
			std::string mtl_file;
			line_stream >> mtl_file;
			loadMtl(mtl_file.c_str());
		}
	}

	if( ! currentMeshName.empty() ) process_mesh();

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

	while( std::getline(mtl_file, line) )
	{
		std::istringstream line_stream(line);
		std::string prefix;
		line_stream >> prefix;

		if( prefix == "newmtl" )
		{
			if( ! currentMaterialName.empty() )
			{
				createMaterial(currentMaterialName, Ka, Kd, Ks, Ns, Ni, d, illum, map_Ks_path, map_Kd_path, map_Ks_path, map_Ns_path, map_d_path, map_bump_path);
				material_names.emplace_back(currentMaterialName);
			}
			line_stream >> currentMaterialName;
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
		else if( prefix == "map_Ka" )
		{
			line_stream >> map_Ka_path;
		}
		else if( prefix == "map_Kd" )
		{
			line_stream >> map_Kd_path;
		}
		else if( prefix == "map_Ks" )
		{
			line_stream >> map_Ks_path;
		}
		else if( prefix == "map_Ns" )
		{
			line_stream >> map_Ns_path;
		}
		else if( prefix == "map_d" )
		{
			line_stream >> map_d_path;
		}
		else if( prefix == "map_bump" )
		{
			line_stream >> map_bump_path;
		}
		else if( prefix == "Ns" )
		{
			line_stream >> Ns;
		}
		else if( prefix == "Ka" )
		{
			line_stream >> Ka.r >> Ka.g >> Ka.b;
		}
		else if( prefix == "Kd" )
		{
			line_stream >> Kd.r >> Kd.g >> Kd.b;
		}
		else if( prefix == "Ks" )
		{
			line_stream >> Ks.r >> Ks.g >> Ks.b;
		}
		else if( prefix == "Ni" )
		{
			line_stream >> Ni;
		}
		else if( prefix == "d" )
		{
			line_stream >> d;
		}
		else if( prefix == "illum" )
		{
			line_stream >> illum;
		}
	}

	if( ! currentMaterialName.empty() )
	{
		createMaterial(currentMaterialName, Ka, Kd, Ks, Ns, Ni, d, illum, map_Ks_path, map_Kd_path, map_Ks_path, map_Ns_path, map_d_path, map_bump_path);
		material_names.emplace_back(currentMaterialName);
	}

	return material_names;
}