#include "pch.hpp"
#include "EngineUtil.hpp"
#include "GraphicsManager.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <functional>
#include <Material.hpp>
#include <Texture.hpp>
#include "Engine/Util/Logging.hpp"


GraphicsManager::~GraphicsManager()
{
	clear();
}

void GraphicsManager::loadResources()
{
	loadShader("Shaders/skybox.vert", "Shaders/skybox.frag", "Skybox");
	loadShader("Shaders/shadowmap.vert", "Shaders/shadowmap.frag", "ShadowMap");
	loadShader("Shaders/pointshadow.vert", "Shaders/pointshadow.frag", "Shaders/pointshadow.geom", "PointShadowMap");
	loadShader("Shaders/hdr.vert", "Shaders/hdr.frag", "HDR");
	loadShader("Shaders/bloomblur.vert", "Shaders/bloomblur.frag", "BloomBlur");
	loadShader("Shaders/gbuffer.vert", "Shaders/gbuffer.frag", "GBuffer");
	loadShader("Shaders/deferred.vert", "Shaders/deferred.frag", "Deferred");
	loadShader("Shaders/ssao.vert", "Shaders/ssao.frag", "SSAO");
	loadShader("Shaders/ssaoblur.vert", "Shaders/ssaoblur.frag", "SSAOBlur");
	loadShader("Shaders/fxaa.vert", "Shaders/fxaa.frag", "FXAA");
	loadShader("Shaders/taa.vert", "Shaders/taa.frag", "TAA");
	loadShader("Shaders/bloomextract.vert", "Shaders/bloomextract.frag", "BloomExtract");
	loadShader("Shaders/ssr.vert", "Shaders/ssr.frag", "SSR");
	loadShader("Shaders/forward.vert", "Shaders/forward.frag", "Forward");
	auto equirectCubemapShader = loadShader("Shaders/cubemap.vert", "Shaders/equirect_to_cubemap.frag", "equirectangularToCubemap");
	auto irradianceShader = loadShader("Shaders/cubemap.vert", "Shaders/irradiance.frag", "irradiance");
	auto prefilterShader = loadShader("Shaders/cubemap.vert", "Shaders/prefilter.frag", "prefilter");
	auto brdfShader = loadShader("Shaders/brdf.vert", "Shaders/brdf.frag", "brdf");

	loadModel("../assets/Helmet/DamagedHelmet.gltf", "Helmet");
	loadModel("../assets/Sponza/Sponza.gltf", "Sponza");
	loadModel("../assets/scene.gltf", "Glass");
	loadEnvironmentMap("default", "../assets/night.hdr", equirectCubemapShader, irradianceShader, prefilterShader, brdfShader);


}


void GraphicsManager::unloadResources()
{
	clear();
}

void GraphicsManager::reloadShaders()
{
	loadShader("Shaders/skybox.vert", "Shaders/skybox.frag", "Skybox");
	loadShader("Shaders/shadowmap.vert", "Shaders/shadowmap.frag", "ShadowMap");
	loadShader("Shaders/pointshadow.vert", "Shaders/pointshadow.frag", "Shaders/pointshadow.geom", "PointShadowMap");
	loadShader("Shaders/hdr.vert", "Shaders/hdr.frag", "HDR");
	loadShader("Shaders/bloomblur.vert", "Shaders/bloomblur.frag", "BloomBlur");
	loadShader("Shaders/gbuffer.vert", "Shaders/gbuffer.frag", "GBuffer");
	loadShader("Shaders/depth.vert", "Shaders/depth.frag", "Depth");
	loadShader("Shaders/deferred.vert", "Shaders/deferred.frag", "Deferred");
	loadShader("Shaders/ssao.vert", "Shaders/ssao.frag", "SSAO");
	loadShader("Shaders/ssaoblur.vert", "Shaders/ssaoblur.frag", "SSAOBlur");
	loadShader("Shaders/fxaa.vert", "Shaders/fxaa.frag", "FXAA");
	loadShader("Shaders/motionblur.vert", "Shaders/motionblur.frag", "MotionBlur");
	loadShader("Shaders/taa.vert", "Shaders/taa.frag", "TAA");
	loadShader("Shaders/bloomextract.vert", "Shaders/bloomextract.frag", "BloomExtract");
	loadShader("Shaders/ssr.vert", "Shaders/ssr.frag", "SSR");
	loadShader("Shaders/forward.vert", "Shaders/forward.frag", "Forward");
}

void GraphicsManager::clear()
{
	mShaderMap.clear();
	mTextureMap.clear();
	mMaterialMap.clear();
	mModelMap.clear();
	mEnvironmentMap.clear();
}

std::shared_ptr<Shader> GraphicsManager::loadShader(std::string_view vertexPath, std::string_view fragmentPath, const std::string& name)
{
	auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
	mShaderMap[name] = shader;
	return shader;
}

std::shared_ptr<Shader> GraphicsManager::loadShader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath,
                                                    const std::string& name)
{
	auto shader = std::make_shared<Shader>(vertexPath, fragmentPath, geometryPath);
	mShaderMap.emplace(name, shader);
	return shader;
}


std::shared_ptr<Shader> GraphicsManager::getShader(const std::string& name)
{
	auto it = mShaderMap.find(name);
	if (it != mShaderMap.end())
	{
		return it->second;
	}
	else
	{
		LOG_WARN(logging::gResourceLogger, "Shader not found: '{}'", name);
		return nullptr;
	}
}


std::shared_ptr<Texture> GraphicsManager::loadTexture(const std::string& path,
													  aiTextureType type,
													  const aiScene* scene)
{
	std::string uniqueKey;

	if ((!path.empty() && path[0] == '*'))
	{
		// Embedded textures often show up as "*0", "*1", etc.
		std::string indexStr = path.substr(1);
		bool isValidIndexFormat = !indexStr.empty() &&
			std::ranges::all_of(indexStr, ::isdigit);

		ASSERT_LOG(logging::gResourceLogger, isValidIndexFormat,
				   "Invalid embedded texture index format: {}", path);
		// If assert continues, format is valid.
		unsigned int textureIndex = std::stoi(indexStr);

		// Construct a unique key: e.g., "Embedded_140535221312672_*0"
		uintptr_t scenePtrVal = reinterpret_cast<uintptr_t>(scene);
		// Consider using std::format for potentially better performance if available (C++20)
		uniqueKey = "Embedded_" + std::to_string(scenePtrVal) + "_" + path;

		// Check cache first
		if (auto it = mTextureMap.find(uniqueKey); it != mTextureMap.end())
		{
			return it->second; // Already loaded
		}

		// Validate scene and index before accessing
		ASSERT_LOG(logging::gResourceLogger, scene != nullptr, "Scene pointer is null for embedded texture: {}", path);
		if (!scene || textureIndex >= scene->mNumTextures)
		{
			LOG_ERROR(logging::gResourceLogger, "Invalid scene or texture index for embedded texture: {}. Scene Texture Count: {}", path, scene ? scene->mNumTextures : 0);
			return nullptr; // Return nullptr here, can't proceed without valid scene/index
		}

		// Load embedded texture from Assimp - Texture constructor should log errors
		aiTexture* aiTex = scene->mTextures[textureIndex];
		auto texture = std::make_shared<Texture>(aiTex);
		// Cache texture even if loading failed internally; renderer handles invalid textures
		mTextureMap.emplace(uniqueKey, texture);
		return texture;
	}
	else // External file path
	{
		std::filesystem::path fsPath(path);
		std::string canonicalPathStr;

		std::error_code ec;
		bool fileExists = std::filesystem::exists(fsPath, ec);
		if (ec)
		{
			LOG_WARN(logging::gResourceLogger, "Error checking existence for '{}': {}. Using original path as key.", path, ec.message());
			canonicalPathStr = path; // Use original path on error
		}
		else if (fileExists)
		{
			std::filesystem::path canonicalPath = std::filesystem::canonical(fsPath, ec);
			if (ec)
			{
				LOG_WARN(logging::gResourceLogger, "Error getting canonical path for '{}': {}. Using original path as key.", path, ec.message());
				canonicalPathStr = path; // Fallback to original path
			}
			else
			{
				canonicalPathStr = canonicalPath.string();
			}
		}
		else
		{
			// File doesn't exist according to std::filesystem::exists.
			// Assimp might still find it relative to the model file. Use the provided path.
			canonicalPathStr = path;
			LOG_TRACE(logging::gResourceLogger, "Texture file '{}' not found via filesystem check. Proceeding with path for Assimp.", path);
		}

		uniqueKey = canonicalPathStr; // Use canonical (or original if failed/not found) path as key

		// Check cache first
		if (auto it = mTextureMap.find(uniqueKey); it != mTextureMap.end())
		{
			return it->second;
		}

		// Load from file - Texture constructor should log errors
		auto texture = std::make_shared<Texture>(uniqueKey);
		// Cache texture even if loading failed internally; renderer handles invalid textures
		mTextureMap.emplace(uniqueKey, texture);
		return texture;
	}
}


std::shared_ptr<Texture> GraphicsManager::getTexture(const std::string& path)
{
	auto it = mTextureMap.find(path);
	if (it != mTextureMap.end())
	{
		return it->second;
	}
	else
	{
		LOG_WARN(logging::gResourceLogger, "Texture not found by path: '{}'", path);
		return nullptr;
	}
}



std::shared_ptr<Material> GraphicsManager::getMaterial(const std::string& name)
{
	auto it = mMaterialMap.find(name);
	if (it != mMaterialMap.end())
	{
		return it->second;
	}
	else
	{
		LOG_WARN(logging::gResourceLogger, "Material not found: '{}'", name);
		return nullptr;
	}
}



std::shared_ptr<Model> GraphicsManager::getModel(const std::string& name)
{
	auto it = mModelMap.find(name);
	if (it != mModelMap.end())
	{
		return it->second;
	}
	else
	{
		LOG_WARN(logging::gResourceLogger, "Model not found: '{}'", name);
		return nullptr;
	}
}

std::shared_ptr<Model> GraphicsManager::loadModel(std::string_view path, std::string name)
{
	// Check if model already loaded
	auto it = mModelMap.find(name);
	if (it != mModelMap.end())
	{
		LOG_INFO(logging::gResourceLogger, "Model '{}' already loaded. Returning cached version.", name);
		return it->second;
	}

	// Load model using Assimp
	Assimp::Importer importer;
	// Optimize mesh data and structure
	const aiScene* scene = importer.ReadFile(path.data(),
											 aiProcess_Triangulate |           // Ensure models are triangles
											 aiProcess_FlipUVs |               // Flip UVs to match OpenGL convention
											 aiProcess_CalcTangentSpace |      // Calculate tangents and bitangents
											 aiProcess_GenSmoothNormals |      // Generate smooth normals if not present
											 aiProcess_JoinIdenticalVertices | // Optimize vertex count
											 aiProcess_OptimizeMeshes |        // Reduce draw calls by merging meshes
											 aiProcess_RemoveRedundantMaterials | // Remove unused materials
											 aiProcess_ValidateDataStructure | // Validate the imported data
											 aiProcess_EmbedTextures         // Load embedded textures if possible (handled in loadTexture)
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_ERROR(logging::gResourceLogger, "Assimp error loading '{}': {}", path, importer.GetErrorString());
		return nullptr;
	}

	// Use std::filesystem for robust path handling
	std::filesystem::path modelPath(path);
	std::string directory = modelPath.parent_path().string();
	if (directory.empty())
	{
		directory = "."; // Use current directory if path has no parent
	}

	// Process the root node recursively
	std::vector<MeshInstance> meshInstances;
	processNode(scene->mRootNode, scene, directory, glm::mat4(1.0f), meshInstances);

	if (meshInstances.empty())
	{
		LOG_WARN(logging::gResourceLogger, "No meshes processed for model '{}' from path '{}'. Check model file or processing logic.", name, path);
	}

	auto model = std::make_shared<Model>(std::move(meshInstances));
	model->setName(name);
	mModelMap.emplace(std::move(name), model); // Use emplace and move name

	LOG_INFO(logging::gResourceLogger, "Successfully loaded model '{}' from '{}'", model->getName(), path);
	return model;
}



std::shared_ptr<Mesh> GraphicsManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
{
	ASSERT_LOG(logging::gResourceLogger, mesh != nullptr, "Received null aiMesh pointer in processMesh.");
	if (!mesh) return nullptr; // Defensive return after assert

	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};

		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		if (mesh->HasNormals())
		{
			vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		else
		{
			// Normals will be zero vector due to value initialization
			LOG_TRACE(logging::gResourceLogger, "Mesh '{}' missing normals at vertex {}.", mesh->mName.C_Str(), i);
		}

		if (mesh->HasTextureCoords(0)) // Assumes only one UV channel
		{
			vertex.texture_coordinates = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		// else: texture_coordinates remains (0.0f, 0.0f)

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}
		// else: tangent/bitangent remain zero vectors

		vertices.push_back(vertex);
	}

	std::vector<unsigned int> indices;
	indices.reserve(mesh->mNumFaces * 3);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			LOG_WARN(logging::gResourceLogger, "Non-triangular face encountered in mesh '{}'. Index count: {}. Skipping face.", mesh->mName.C_Str(), face.mNumIndices);
			continue; // Skip non-triangles
		}
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	std::shared_ptr<Material> material = nullptr;
	if (mesh->mMaterialIndex >= 0 && scene->HasMaterials()) // Check if material index is valid and materials exist
	{
		// Ensure index is within bounds
		if (mesh->mMaterialIndex < scene->mNumMaterials)
		{
			aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];
			material = loadMaterial(ai_material, directory, scene);
			if (!material)
			{
				LOG_WARN(logging::gResourceLogger, "Failed to load material for mesh '{}' (Material Index: {}). Mesh will have no material.", mesh->mName.C_Str(), mesh->mMaterialIndex);
				// Assign a default material or handle appropriately? For now, it remains nullptr.
			}
		}
		else
		{
			LOG_ERROR(logging::gResourceLogger, "Mesh '{}' has out-of-bounds material index ({}), scene has {} materials.", mesh->mName.C_Str(), mesh->mMaterialIndex, scene->mNumMaterials);
		}
	}
	else if (!scene->HasMaterials())
	{
		LOG_WARN(logging::gResourceLogger, "Mesh '{}' references material index {} but scene has no materials.", mesh->mName.C_Str(), mesh->mMaterialIndex);
	}


	auto new_mesh = std::make_shared<Mesh>(std::move(vertices), std::move(indices), material);

	std::string mesh_name = mesh->mName.C_Str();
	if (mesh_name.empty())
	{
		// Generate a placeholder name if Assimp didn't provide one
		mesh_name = "unnamed_mesh_" + std::to_string(reinterpret_cast<uintptr_t>(mesh));
		LOG_TRACE(logging::gResourceLogger, "Assigning generated name to unnamed mesh: {}", mesh_name);
	}
	new_mesh->setName(mesh_name);


	return new_mesh;
}

std::shared_ptr<Material> GraphicsManager::loadMaterial(aiMaterial* mat, const std::string& directory, const aiScene* scene)
{
	ASSERT_LOG(logging::gResourceLogger, mat != nullptr, "Received null aiMaterial pointer in loadMaterial.");
	if (!mat) return nullptr;

	aiString name;
	mat->Get(AI_MATKEY_NAME, name);
	std::string material_name_str = name.C_Str();

	if (material_name_str.empty())
	{
		uintptr_t matPtrVal = reinterpret_cast<uintptr_t>(mat);
		material_name_str = "material_" + std::to_string(matPtrVal);
		LOG_WARN(logging::gResourceLogger, "Material has no name. Generating unique name: {}", material_name_str);
	}

	// Check cache
	if (auto it = mMaterialMap.find(material_name_str); it != mMaterialMap.end())
	{
		return it->second;
	}

	auto material = std::make_shared<Material>();
	material->setName(material_name_str); // Set name early

	aiColor4D color;
	float float_val;

	if (aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &color) == AI_SUCCESS)
	{
		material->setAlbedo(glm::vec4(color.r, color.g, color.b, color.a));
	} // Fallback to legacy diffuse
	else if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
	{
		material->setAlbedo(glm::vec4(color.r, color.g, color.b, color.a));
		LOG_INFO(logging::gResourceLogger, "Using legacy diffuse color for albedo in material '{}'", material_name_str);
	}
	else
	{
		material->setAlbedo(glm::vec4(1.0f)); // Default albedo
	}

	// Metallic factor (PBR)
	if (aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &float_val) == AI_SUCCESS)
	{
		material->setMetallic(float_val);
	}
	else
	{
		material->setMetallic(0.0f); // Default metallic
	}

	// Roughness factor (PBR)
	if (aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &float_val) == AI_SUCCESS)
	{
		material->setRoughness(float_val);
	}
	else
	{
		material->setRoughness(0.5f); // Default roughness
	}

	// Opacity (Common)
	if (aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &float_val) == AI_SUCCESS)
	{
		material->setOpacity(float_val);
		// Update albedo alpha if opacity differs from base color alpha
		if (material->getAlbedo().a != float_val)
		{
			glm::vec4 currentAlbedo = material->getAlbedo();
			material->setAlbedo(glm::vec4(currentAlbedo.r, currentAlbedo.g, currentAlbedo.b, float_val));
			LOG_TRACE(logging::gResourceLogger, "Updating material '{}' albedo alpha based on opacity property.", material_name_str);
		}
	}
	else
	{
		// If opacity key isn't present, assume albedo alpha is the intended opacity
		material->setOpacity(material->getAlbedo().a);
	}

	// Emissive properties (Common)
	if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color) == AI_SUCCESS)
	{
		material->setEmissiveColor(glm::vec3(color.r, color.g, color.b));
	}
	else
	{
		material->setEmissiveColor(glm::vec3(0.0f)); // Default emissive color
	}

	if (aiGetMaterialFloat(mat, AI_MATKEY_EMISSIVE_INTENSITY, &float_val) == AI_SUCCESS)
	{
		material->setEmissiveIntensity(float_val);
	}
	else
	{
		material->setEmissiveIntensity(material->getEmissiveColor() == glm::vec3(0.0f) ? 0.0f : 1.0f); // Default intensity (1.0 if color is set, else 0.0)
	}

	// Reflectivity (Legacy, less common in PBR)
	if (aiGetMaterialFloat(mat, AI_MATKEY_REFLECTIVITY, &float_val) == AI_SUCCESS)
	{
		material->setReflectivity(float_val);
	}
	else
	{
		material->setReflectivity(0.0f); // Default reflectivity
	}

	auto loadTextureType = [&](aiTextureType texType) -> std::shared_ptr<Texture>
		{
			auto textures = loadMaterialTextures(mat, texType, directory, scene);
			if (!textures.empty())
			{
				if (textures.size() > 1)
				{
					// Cast texType to int for logging
					LOG_WARN(logging::gResourceLogger, "Material '{}' has multiple textures of type {}. Using the first one.", material_name_str, static_cast<int>(texType));
				}
				return textures[0]; // Return first loaded texture of this type
			}
			return nullptr;
		};

	// Albedo/Base Color Texture
	material->setAlbedoTexture(loadTextureType(aiTextureType_BASE_COLOR));
	if (!material->getAlbedoTexture())
	{ // Fallback to Diffuse
		material->setAlbedoTexture(loadTextureType(aiTextureType_DIFFUSE));
		if (material->getAlbedoTexture())
		{
			LOG_INFO(logging::gResourceLogger, "Using legacy diffuse texture for albedo in material '{}'", material_name_str);
		}
	}

	// Normal Map Texture
	material->setNormalTexture(loadTextureType(aiTextureType_NORMALS));
	if (!material->getNormalTexture())
	{ // Fallback to Height Map (some exporters use this)
		material->setNormalTexture(loadTextureType(aiTextureType_HEIGHT));
		if (material->getNormalTexture())
		{
			LOG_INFO(logging::gResourceLogger, "Using height texture as normal map in material '{}'", material_name_str);
		}
	}



	// AO Texture (Check standard AO then Lightmap as fallback common in glTF)
	material->setAOTexture(loadTextureType(aiTextureType_AMBIENT_OCCLUSION));
	if (!material->getAOTexture())
	{
		material->setAOTexture(loadTextureType(aiTextureType_LIGHTMAP));
		if (material->getAOTexture())
		{
			LOG_INFO(logging::gResourceLogger, "Using lightmap texture as AO map in material '{}'", material_name_str);
		}
	}

	// Emissive Texture
	material->setEmissiveTexture(loadTextureType(aiTextureType_EMISSIVE));
	if (!material->getEmissiveTexture())
	{ // Fallback
		material->setEmissiveTexture(loadTextureType(aiTextureType_EMISSION_COLOR));
	}



	material->setMetalRoughTexture(loadTextureType(aiTextureType_UNKNOWN));


	// PBR Textures
	if (!material->getMetalRoughTexture())
	{
		material->setMetallicTexture(loadTextureType(aiTextureType_METALNESS));
		material->setRoughnessTexture(loadTextureType(aiTextureType_DIFFUSE_ROUGHNESS)); // Correct Assimp type for roughness
	}



	// Store in material map
	mMaterialMap.emplace(material_name_str, material); // Use the original string key

	return material;
}



std::vector<std::shared_ptr<Texture>> GraphicsManager::loadMaterialTextures(
	const aiMaterial* mat,
	aiTextureType type,
	const std::string& directory,
	const aiScene* scene)
{
	std::vector<std::shared_ptr<Texture>> textures;
	unsigned int textureCount = mat->GetTextureCount(type);
	if (textureCount == 0) return textures; // Early exit if no textures of this type

	textures.reserve(textureCount);

	for (unsigned int i = 0; i < textureCount; i++)
	{
		aiString ai_path;
		// Get texture path/embedded ID
		if (mat->GetTexture(type, i, &ai_path) != AI_SUCCESS)
		{
			// Cast type to int for logging
			LOG_WARN(logging::gResourceLogger, "Failed to get texture path for type {} at index {} in material '{}'", static_cast<int>(type), i, mat->GetName().C_Str());
			continue; // Skip this texture
		}

		std::string texture_path_str = ai_path.C_Str();
		std::string full_path_or_key;

		if ((!texture_path_str.empty() && texture_path_str[0] == '*'))
		{
			// Embedded texture: Pass the "*" identifier directly to loadTexture
			full_path_or_key = texture_path_str;
		}
		else
		{
			// External texture: Construct full path relative to model directory
			try
			{
				std::filesystem::path dirPath(directory);
				std::filesystem::path texPath(texture_path_str);
				std::filesystem::path combinedPath;

				if (texPath.is_absolute())
				{
					combinedPath = texPath;
				}
				else
				{
					// Handle potential ..\\.. paths by normalizing
					combinedPath = (dirPath / texPath).lexically_normal();
				}
				// Use the normalized path string
				full_path_or_key = combinedPath.string();
			}
			catch (const std::exception& e)
			{
				LOG_ERROR(logging::gResourceLogger, "Filesystem error processing texture path '{}' in directory '{}': {}", texture_path_str, directory, e.what());
				continue; // Skip this texture on path error
			}
		}

		// Use the central loadTexture function (handles caching, embedded/external logic)

		if (auto texture = loadTexture(full_path_or_key, type, scene))
		{
			if (type == aiTextureType_NORMALS || type == aiTextureType_HEIGHT || type == aiTextureType_DISPLACEMENT)
			{
				texture->setNormalSamplerParameters();
			}
			textures.push_back(texture);
		}
		else
		{
			// loadTexture already logs errors, but add context
			// Cast type to int for logging
			LOG_WARN(logging::gResourceLogger, "Failed attempt to load/retrieve texture (type {}, index {}) with path/key: '{}' for material '{}'", static_cast<int>(type), i, full_path_or_key, mat->GetName().C_Str());
		}
	}
	return textures;
}

std::shared_ptr<EnvironmentMap> GraphicsManager::loadEnvironmentMap(std::string name, std::string_view hdrPath,
                                                                    const std::shared_ptr<Shader>&
                                                                    equirectangularToCubemapShader, const std::shared_ptr<Shader>
                                                                    & irradianceShader, const std::shared_ptr<Shader>&
                                                                    prefilterShader, const std::shared_ptr<Shader>&
                                                                    brdfShader)
{
	auto it = mEnvironmentMap.find(name);
	if (it != mEnvironmentMap.end())
	{
		LOG_INFO(logging::gResourceLogger, "Environment map '{}' already loaded. Returning cached version.", name);
		return it->second;
	}

	// Validate required shaders (check for null pointers)
	// relies on Shader constructor logging errors if compilation fails
	if (!equirectangularToCubemapShader || !irradianceShader || !prefilterShader || !brdfShader)
	{
		LOG_ERROR(logging::gResourceLogger, "Cannot load environment map '{}'. One or more required shaders are null.", name);
		return nullptr;
	}

	// EnvironmentMap constructor should log errors if it fails internally
	auto environmentMap = std::make_shared<EnvironmentMap>(hdrPath, equirectangularToCubemapShader, irradianceShader, prefilterShader, brdfShader);

	// Cache the map regardless of internal loading status; renderer handles invalid maps
	mEnvironmentMap.emplace(name, environmentMap); // Use name directly for caching
	LOG_INFO(logging::gResourceLogger, "Successfully loaded environment map '{}' from '{}'", name, hdrPath);
	return environmentMap;
}

std::shared_ptr<EnvironmentMap> GraphicsManager::getEnvironmentMap(const std::string& name)
{
	auto it = mEnvironmentMap.find(name);
	if (it != mEnvironmentMap.end())
	{
		return it->second;
	}
	else
	{
		LOG_WARN(logging::gResourceLogger, "EnvironmentMap not found: '{}'", name);
		return nullptr;
	}
}

void GraphicsManager::processNode(const aiNode* node, const aiScene* scene, const std::string& directory, const glm::mat4& parentTransform, std::vector<MeshInstance>& meshInstances)
{
	ASSERT_LOG(logging::gResourceLogger, node != nullptr, "Received null aiNode pointer in processNode.");
	ASSERT_LOG(logging::gResourceLogger, scene != nullptr, "Received null aiScene pointer in processNode.");
	if (!node || !scene) return; // Defensive return after assert

	glm::mat4 nodeTransform = parentTransform * aiMatrixToGlm(node->mTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		unsigned int meshIndex = node->mMeshes[i];
		// Validate mesh index
		if (meshIndex >= scene->mNumMeshes)
		{
			LOG_ERROR(logging::gResourceLogger, "Node '{}' contains invalid mesh index: {}. Scene mesh count: {}. Skipping mesh.", node->mName.C_Str(), meshIndex, scene->mNumMeshes);
			continue;
		}

		aiMesh* ai_mesh = scene->mMeshes[meshIndex];
		if (auto mesh = processMesh(ai_mesh, scene, directory))
		{
			meshInstances.emplace_back(mesh, nodeTransform); // Use emplace_back with constructor
		}
		else
		{
			LOG_WARN(logging::gResourceLogger, "Failed to process mesh at index {} referenced by node '{}'. Skipping mesh instance.", meshIndex, node->mName.C_Str());
		}
	}

	// Recursively process children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		// Pass current node's combined transform to children
		processNode(node->mChildren[i], scene, directory, nodeTransform, meshInstances);
	}
}

glm::mat4 GraphicsManager::aiMatrixToGlm(const aiMatrix4x4& aiMat)
{
	glm::mat4 mat;
	// aiMatrix4x4 is row-major, glm::mat4 is column-major.
	// Transpose during conversion.
	mat[0][0] = aiMat.a1; mat[1][0] = aiMat.b1; mat[2][0] = aiMat.c1; mat[3][0] = aiMat.d1;
	mat[0][1] = aiMat.a2; mat[1][1] = aiMat.b2; mat[2][1] = aiMat.c2; mat[3][1] = aiMat.d2;
	mat[0][2] = aiMat.a3; mat[1][2] = aiMat.b3; mat[2][2] = aiMat.c3; mat[3][2] = aiMat.d3;
	mat[0][3] = aiMat.a4; mat[1][3] = aiMat.b4; mat[2][3] = aiMat.c4; mat[3][3] = aiMat.d4;
	return mat;
}