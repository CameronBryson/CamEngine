#include "GraphicsManager.hpp"

#include <Engine/Util/EngineUtil.hpp>
#include <fstream>
#include <functional>
#include <iostream>

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Util/OpenGLUtil.hpp"
#include "stb_image.h"


GraphicsManager::~GraphicsManager() { Clear(); }

void GraphicsManager::loadResources() 
{
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/3D_texture.frag", "3D_texture");
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/3D_color.frag", "3D_color");
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/2D_color.frag", "2D_texture");
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/2D_color.frag", "2D_color");
	loadShader("sources/Shaders/text.vert", "sources/Shaders/text.frag", "text");

	loadFont("assets/Font/arial.ttf", 48, "arial");
	loadModel(engine_util::buildPath("assets/f40.obj"), "bottle");
	loadModel(engine_util::buildPath("assets/Ship.obj"), "player");

	loadModel(engine_util::buildPath("assets/sphere.obj"), "sphere");
	loadModel(engine_util::buildPath("assets/cube.obj"), "cube");
	loadModel(engine_util::buildPath("assets/quad.obj"), "quad");
	loadModel(engine_util::buildPath("assets/skybox.obj"), "skybox");
	loadModel(engine_util::buildPath("assets/asteroid.obj"), "asteroid");
	loadModel(engine_util::buildPath("assets/sat.obj"), "sat");
	loadModel(engine_util::buildPath("assets/enemy_ship.obj"), "enemy");
}

void GraphicsManager::unloadResources() { Clear(); }

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& vShaderFile,
                                                    const std::string& fShaderFile,
                                                    const std::string& name)
{
	auto shader = Shader::createShader(vShaderFile, fShaderFile);
	shader_map.emplace(name, shader);
	return shader;
}

std::shared_ptr<Shader> GraphicsManager::getShader(const std::string& name) { return shader_map.at(name); }

std::shared_ptr<Texture> GraphicsManager::loadTexture(const std::string& file,
                                                      aiTextureType type,
                                                      const std::string& name)
{
	auto texture = Texture::createTexture(file, type);
	texture_map.emplace(name, texture);
	return texture;
}


std::shared_ptr<Texture> GraphicsManager::getTexture(const std::string& name) { return texture_map.at(name); }

std::shared_ptr<Mesh> GraphicsManager::createMesh(const std::string& name,
                                                  const std::vector<Vertex>& vertices,
                                                  const std::vector<unsigned>& indices,
                                                  const std::string& material_name)
{
	auto mesh = Mesh::createMesh(vertices, indices, material_name);
	mesh_map.emplace(name, mesh);
	return mesh;
}

std::shared_ptr<Mesh> GraphicsManager::getMesh(const std::string& name) { return mesh_map.at(name); }

std::shared_ptr<Material> GraphicsManager::createMaterial(const std::string& name,
                                                          glm::vec3 ambient,
                                                          glm::vec3 diffuse,
                                                          glm::vec3 specular,
	glm::vec3 emmisive,
                                                          float shininess,
                                                          float opticalDensity,
                                                          float transparency,
	int illum,
                                                          std::shared_ptr<Texture> ambientMap,
                                                          std::shared_ptr<Texture> diffuseMap,
                                                          std::shared_ptr<Texture> specularMap,
                                                          std::shared_ptr<Texture> normalMap,
														std::shared_ptr<Texture> roughnessMap)
{
	auto material = Material::createMaterial(ambient, diffuse, specular, emmisive, shininess,opticalDensity, transparency, illum, ambientMap,
	                                         diffuseMap, specularMap, normalMap,roughnessMap);
	material_map.emplace(name, material);
	return material;
}


std::shared_ptr<Material> GraphicsManager::getMaterial(const std::string& name) { return material_map.at(name); }

std::shared_ptr<Model> GraphicsManager::createModel(const std::vector<std::string>& mesh_names, const std::string& name)
{
	auto model = Model::createModel(mesh_names);
	model_map.emplace(name, model);
	return model;
}


std::shared_ptr<Model> GraphicsManager::getModel(const std::string& name) { return model_map.at(name); }

std::shared_ptr<Font> GraphicsManager::loadFont(const std::string& fontPath,
                                                float fontSize,
                                                const std::string& fontName)
{
	auto font = std::make_shared<Font>(fontPath, fontSize);
	font_map.emplace(fontName, font);
	return font;
}

std::shared_ptr<Font> GraphicsManager::getFont(const std::string& name) { return font_map.at(name); }

void GraphicsManager::Clear()
{
	shader_map.clear();
	texture_map.clear();
	mesh_map.clear();
	model_map.clear();
	material_map.clear();
}


std::shared_ptr<Model> GraphicsManager::loadModel(const std::string& file, const std::string& name)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs |
	                                                   aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes |
	                                                   aiProcess_GenSmoothNormals | aiProcess_ValidateDataStructure);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}
	processNode(scene->mRootNode, scene);
	std::vector<std::string> mesh_names;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		std::string mesh_name = scene->mMeshes[i]->mName.C_Str();
		if (mesh_name.empty())
		{
			mesh_name = name + "_mesh_" + std::to_string(i);
		}
		mesh_names.push_back(mesh_name);
	}
	// delete scene?
	return createModel(mesh_names, name);
}

void GraphicsManager::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void GraphicsManager::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Extract mesh name
	std::string mesh_name = mesh->mName.C_Str();
	if (mesh_name.empty())
	{
		mesh_name = "mesh_" + std::to_string(mesh_map.size());
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	// Check if mesh has tangents and bitangents
	bool hasTangents = mesh->HasTangentsAndBitangents();

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// Positions
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		// Normals
		if (mesh->HasNormals())
		{
			vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		else
		{
			vertex.normal = glm::vec3(0.0f);
		}

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Check if the mesh contains texture coordinates
		{
			vertex.texture_coordinates = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.texture_coordinates = glm::vec2(0.0f);
		}

		// Tangents
		if (hasTangents)
		{
			vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		}
		else
		{
			vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f); // Default tangent
		}

		// Bitangents
		if (hasTangents)
		{
			vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}
		else
		{
			vertex.bitangent = glm::vec3(0.0f, 1.0f, 0.0f); // Default bitangent
		}

		vertices.push_back(vertex);
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	std::string material_name = "Default";
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		material_name = processMaterial(material, scene);
	}

	// Create and store the mesh
	createMesh(mesh_name, vertices, indices, material_name);
}

std::string GraphicsManager::processMaterial(aiMaterial* material, const aiScene* scene)
{
	// Get the material name
	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	std::string material_name = name.C_Str();

	// Check if the material already exists
	if (material_map.find(material_name) != material_map.end())
	{
		return material_name;
	}

	// Initialize material properties with reasonable defaults
	glm::vec3 ambient(0.1f, 0.1f, 0.1f);  // Ambient color
	glm::vec3 diffuse(0.5f, 0.5f, 0.5f);  // Diffuse color
	glm::vec3 specular(1.0f, 1.0f, 1.0f); // Specular color
	glm::vec3 emissive(0.0f, 0.0f, 0.0f); // Emissive color
	float shininess = 32.0f;              // Shininess
	float opticalDensity = 1.0f;          // Optical density (index of refraction)
	float transparency = 1.0f;            // Transparency factor
	int illum = 2;                        // Illumination model

	// Retrieve material colors
	aiColor3D color(0.0f, 0.0f, 0.0f);
	if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
	{
		ambient = glm::vec3(color.r, color.g, color.b);
	}
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
	{
		diffuse = glm::vec3(color.r, color.g, color.b);
	}
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
	{
		specular = glm::vec3(color.r, color.g, color.b);
	}
	if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
	{
		emissive = glm::vec3(color.r, color.g, color.b);
	}

	// Retrieve scalar properties
	material->Get(AI_MATKEY_SHININESS, shininess);
	material->Get(AI_MATKEY_REFRACTI, opticalDensity);
	material->Get(AI_MATKEY_OPACITY, transparency);
	material->Get(AI_MATKEY_SHADING_MODEL, illum);

	// Initialize textures
	std::shared_ptr<Texture> ambientMap = nullptr;
	std::shared_ptr<Texture> diffuseMap = nullptr;
	std::shared_ptr<Texture> specularMap = nullptr;
	std::shared_ptr<Texture> normalMap = nullptr;
	std::shared_ptr<Texture> roughnessMap = nullptr;

	// Helper lambda to load a texture of a given type
	auto loadTextureOfType = [this, material](aiTextureType type,
	                                          const std::string& typeName) -> std::shared_ptr<Texture>
	{
		if (material->GetTextureCount(type) > 0)
		{
			aiString texPath;
			material->GetTexture(type, 0, &texPath);
			std::string path = texPath.C_Str();
			std::cout << "Loading " << typeName << " texture: " << path << std::endl;

			// Check if texture is already loaded
			if (texture_map.find(path) != texture_map.end())
			{
				return texture_map[path];
			}
			std::shared_ptr<Texture> texture = loadTexture(path, type, path);
			texture_map[path] = texture;

			return texture;
		}
		return nullptr;
	};

	// Load textures
	ambientMap = loadTextureOfType(aiTextureType_AMBIENT, "ambient");
	diffuseMap = loadTextureOfType(aiTextureType_DIFFUSE, "diffuse");
	specularMap = loadTextureOfType(aiTextureType_SPECULAR, "specular");
	normalMap = loadTextureOfType(aiTextureType_NORMALS, "normal");
	// If normal map not found under aiTextureType_NORMALS, try aiTextureType_HEIGHT
	if (!normalMap)
	{
		normalMap = loadTextureOfType(aiTextureType_HEIGHT, "height (used as normal)");
	}
	roughnessMap = loadTextureOfType(aiTextureType_DIFFUSE_ROUGHNESS, "roughness");

	// Create and store the material with the new properties
	createMaterial(material_name, ambient, diffuse, specular, emissive, shininess, opticalDensity, transparency, illum,
	               ambientMap, diffuseMap, specularMap, normalMap, roughnessMap);

	return material_name;
}
