// GraphicsManager.cpp
#include "Engine/pch.hpp"
#include "GraphicsManager.hpp"

#include <Engine/Util/EngineUtil.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "Engine/Graphics/Font.hpp"
#include "Engine/Graphics/Material.hpp"
#include "Engine/Graphics/Mesh.hpp"
#include "Engine/Graphics/Model.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Util/OpenGLUtil.hpp"
#include "stb_image.h"
#include "glm/vec4.hpp"

// Enum for different texture roles
enum class TextureRole
{
	Diffuse,
	Specular,
	Albedo,
	Metalness,
	Roughness,
	AmbientOcclusion,
	Normal,
	Emissive
};

// Utility function to map aiTextureType to TextureRole
std::optional<TextureRole> GetTextureRole(aiTextureType type)
{
	switch (type)
	{
		case aiTextureType_DIFFUSE:
			return TextureRole::Diffuse;
		case aiTextureType_SPECULAR:
			return TextureRole::Specular;
		case aiTextureType_BASE_COLOR:
			return TextureRole::Albedo;
		case aiTextureType_METALNESS:
			return TextureRole::Metalness;
		case aiTextureType_DIFFUSE_ROUGHNESS:
			return TextureRole::Roughness;
		case aiTextureType_AMBIENT_OCCLUSION:
			return TextureRole::AmbientOcclusion;
		case aiTextureType_EMISSIVE:
			return TextureRole::Emissive;
		default:
			return std::nullopt;
	}
}

GraphicsManager::~GraphicsManager() { Clear(); }

void GraphicsManager::loadResources()
{
	/*loadShader("sources/Shaders/vertex.vert", "sources/Shaders/3D_texture.frag", "3D_texture");
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/3D_color.frag", "3D_color");
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/2D_color.frag", "2D_texture");
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/2D_color.frag", "2D_color");
	loadShader("sources/Shaders/text.vert", "sources/Shaders/text.frag", "text");*/
	loadShader("sources/Shaders/vertex.vert", "sources/Shaders/PBR.frag", "PBR");

	// loadFont("assets/Font/arial.ttf", 48, "arial");
	loadModel(engine_util::buildPath("assets/scene.gltf"), "bottle");
	/*loadModel(engine_util::buildPath("assets/Ship.obj"), "player");

	loadModel(engine_util::buildPath("assets/sphere.obj"), "sphere");
	loadModel(engine_util::buildPath("assets/cube.obj"), "cube");
	loadModel(engine_util::buildPath("assets/quad.obj"), "quad");
	loadModel(engine_util::buildPath("assets/skybox.obj"), "skybox");
	loadModel(engine_util::buildPath("assets/asteroid.obj"), "asteroid");
	loadModel(engine_util::buildPath("assets/sat.obj"), "sat");
	loadModel(engine_util::buildPath("assets/enemy_ship.obj"), "enemy");*/
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

std::shared_ptr<Shader> GraphicsManager::getShader(const std::string& name)
{
	auto it = shader_map.find(name);
	if (it != shader_map.end())
	{
		return it->second;
	}
	std::cerr << "Shader not found: " << name << std::endl;
	return nullptr;
}

std::shared_ptr<Texture> GraphicsManager::loadTexture(const std::string& file,
                                                      aiTextureType type,
                                                      const std::string& name)
{
	// Check if texture is already loaded
	auto it = texture_map.find(name);
	if (it != texture_map.end())
	{
		return it->second;
	}

	auto texture = Texture::createTexture(file, type);
	if (texture)
	{
		texture_map.emplace(name, texture);
	}
	else
	{
		std::cerr << "Failed to load texture: " << file << std::endl;
	}
	return texture;
}

std::shared_ptr<Texture> GraphicsManager::getTexture(const std::string& name)
{
	auto it = texture_map.find(name);
	if (it != texture_map.end())
	{
		return it->second;
	}
	std::cerr << "Texture not found: " << name << std::endl;
	return nullptr;
}

std::shared_ptr<Mesh> GraphicsManager::createMesh(const std::string& name,
                                                  const std::vector<Vertex>& vertices,
                                                  const std::vector<unsigned>& indices,
                                                  const std::string& material_name)
{
	auto mesh = Mesh::createMesh(vertices, indices, material_name);
	mesh_map.emplace(name, mesh);
	return mesh;
}

std::shared_ptr<Mesh> GraphicsManager::getMesh(const std::string& name)
{
	auto it = mesh_map.find(name);
	if (it != mesh_map.end())
	{
		return it->second;
	}
	std::cerr << "Mesh not found: " << name << std::endl;
	return nullptr;
}

std::shared_ptr<Material> GraphicsManager::createMaterial(const std::string& name,
                                                          const glm::vec4& albedo,
                                                          float metallic,
                                                          float roughness,
                                                          float AO,
                                                          std::shared_ptr<Texture> albedoTexture,
                                                          std::shared_ptr<Texture> normalTexture,
                                                          std::shared_ptr<Texture> metallicTexture,
                                                          std::shared_ptr<Texture> roughNessTexture,
                                                          std::shared_ptr<Texture> AOTexture,
                                                          std::shared_ptr<Texture> emissiveTexture)
{
	auto material = Material::createMaterial(albedo, metallic, roughness, AO, albedoTexture, normalTexture,
	                                         metallicTexture, roughNessTexture, AOTexture, emissiveTexture);
	material_map.emplace(name, material);
	return material;
}

std::shared_ptr<Material> GraphicsManager::getMaterial(const std::string& name)
{
	auto it = material_map.find(name);
	if (it != material_map.end())
	{
		return it->second;
	}
	std::cerr << "Material not found: " << name << std::endl;
	return nullptr;
}

std::shared_ptr<Model> GraphicsManager::createModel(const std::vector<std::string>& mesh_names, const std::string& name)
{
	auto model = Model::createModel(mesh_names);
	model_map.emplace(name, model);
	return model;
}

std::shared_ptr<Model> GraphicsManager::getModel(const std::string& name)
{
	auto it = model_map.find(name);
	if (it != model_map.end())
	{
		return it->second;
	}
	std::cerr << "Model not found: " << name << std::endl;
	return nullptr;
}

std::shared_ptr<Font> GraphicsManager::loadFont(const std::string& fontPath,
                                                float fontSize,
                                                const std::string& fontName)
{
	// Check if font is already loaded
	auto it = font_map.find(fontName);
	if (it != font_map.end())
	{
		return it->second;
	}

	auto font = std::make_shared<Font>(fontPath, fontSize);
	if (font)
	{
		font_map.emplace(fontName, font);
	}
	else
	{
		std::cerr << "Failed to load font: " << fontPath << std::endl;
	}
	return font;
}

std::shared_ptr<Font> GraphicsManager::getFont(const std::string& name)
{
	auto it = font_map.find(name);
	if (it != font_map.end())
	{
		return it->second;
	}
	std::cerr << "Font not found: " << name << std::endl;
	return nullptr;
}

void GraphicsManager::Clear()
{
	shader_map.clear();
	texture_map.clear();
	mesh_map.clear();
	model_map.clear();
	material_map.clear();
	font_map.clear();
}

std::shared_ptr<Model> GraphicsManager::loadModel(const std::string& file, const std::string& name)
{
	Assimp::Importer importer;
	const aiScene* scene =
	    importer.ReadFile(file, aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes |
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
	// Then do the same for each of its children
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
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// Positions
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		// Normals
		vertex.normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) :
		                                     glm::vec3(0.0f);

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
		vertex.tangent = hasTangents ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z) :
		                               glm::vec3(1.0f, 0.0f, 0.0f); // Default tangent

		// Bitangents
		vertex.bitangent = hasTangents ?
		                       glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z) :
		                       glm::vec3(0.0f, 1.0f, 0.0f); // Default bitangent

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
	if (material_name.empty())
	{
		material_name = "Material_" + std::to_string(material_map.size());
	}

	// Check if the material already exists
	if (material_map.find(material_name) != material_map.end())
	{
		return material_name;
	}

	// Initialize PBR material properties with default values
	glm::vec4 albedo(0.0f, 0.0f, 0.0f,0.0f);   // Albedo color
	float metallic = 0.0f;                // Metallic factor
	float roughness = 0.0f;               // Roughness factor
	float AO = 0.0f;                      // Ambient Occlusion

	// Retrieve material colors (if available)
	aiColor3D color(0.0f, 0.0f, 0.0f);
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
	{
		albedo = glm::vec4(color.r, color.g, color.b,1);
	}

	// Retrieve scalar properties (if available)
	material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
	material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
	material->Get(AI_MATKEY_OPACITY, AO); // Using opacity as AO factor for demonstration

	// Load PBR textures
	std::shared_ptr<Texture> albedoMap = nullptr;
	std::shared_ptr<Texture> normalMap = nullptr;
	std::shared_ptr<Texture> metallicMap = nullptr;
	std::shared_ptr<Texture> roughnessMap = nullptr;
	std::shared_ptr<Texture> AOMap = nullptr;
	std::shared_ptr<Texture> emissiveMap = nullptr;

	// Iterate through all texture types supported by Assimp
	for (unsigned int type = 0; type < aiTextureType_UNKNOWN; ++type)
	{
		aiTextureType aiType = static_cast<aiTextureType>(type);
		auto roleOpt = GetTextureRole(aiType);
		if (roleOpt.has_value())
		{
			TextureRole role = roleOpt.value();
			if (material->GetTextureCount(aiType) > 0)
			{
				aiString texPath;
				if (material->GetTexture(aiType, 0, &texPath) == AI_SUCCESS)
				{
					std::string path = texPath.C_Str();
					std::cout << "Loading texture (" << static_cast<int>(role) << "): " << path << std::endl;

					// Check if texture is embedded
					const aiTexture* embeddedTex = scene->GetEmbeddedTexture(path.c_str());
					std::shared_ptr<Texture> texture = nullptr;

					if (embeddedTex)
					{
						// Handle embedded texture
						printf("Cant handle embedded textures\n");
					}
					else
					{
						// Handle texture from file
						texture = loadTexture(path, aiType, path);
						if (!texture)
						{
							std::cerr << "Failed to load texture from file: " << path << std::endl;
							continue;
						}
					}

					// Assign texture to the appropriate map
					switch (role)
					{
						case TextureRole::Albedo:
							albedoMap = texture;
							break;
						case TextureRole::Metalness:
							metallicMap = texture;
							break;
						case TextureRole::Roughness:
							roughnessMap = texture;
							break;
						case TextureRole::AmbientOcclusion:
							AOMap = texture;
							break;
						case TextureRole::Normal:
							normalMap = texture;
							break;
						case TextureRole::Emissive:
							emissiveMap = texture;
							break;
						case TextureRole::Diffuse:
						case TextureRole::Specular:
						default:
							// Handle other texture roles if necessary
							break;
					}
				}
			}
		}
	}

	// Create and store the PBR material
	createMaterial(material_name, albedo, metallic, roughness, AO, albedoMap, normalMap, metallicMap, roughnessMap,
	               AOMap, emissiveMap);

	return material_name;
}
