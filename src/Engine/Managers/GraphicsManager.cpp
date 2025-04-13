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
#include <string_view> // Include for potential future use


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
	loadShader("Shaders/depth.vert", "Shaders/depth.frag", "Depth");
	loadShader("Shaders/deferred.vert", "Shaders/deferred.frag", "Deferred");
	loadShader("Shaders/ssao.vert", "Shaders/ssao.frag", "SSAO");
	loadShader("Shaders/ssaoblur.vert", "Shaders/ssaoblur.frag", "SSAOBlur");
	loadShader("Shaders/fxaa.vert", "Shaders/fxaa.frag", "FXAA");
	loadShader("Shaders/motionblur.vert", "Shaders/motionblur.frag", "MotionBlur");
	loadShader("Shaders/taa.vert", "Shaders/taa.frag", "TAA");
	loadShader("Shaders/bloomextract.vert", "Shaders/bloomextract.frag", "BloomExtract");
	loadShader("Shaders/luminance.comp", "Luminance");
	loadShader("Shaders/adaptation.comp", "Adaptation");
	loadShader("Shaders/ssr.vert", "Shaders/ssr.frag", "SSR");
	auto equirectCubemapShader = loadShader("Shaders/cubemap.vert", "Shaders/equirect_to_cubemap.frag", "equirectangularToCubemap");
	auto irradianceShader = loadShader("Shaders/cubemap.vert", "Shaders/irradiance.frag", "irradiance");
	auto prefilterShader = loadShader("Shaders/cubemap.vert", "Shaders/prefilter.frag", "prefilter");
	auto brdfShader = loadShader("Shaders/brdf.vert", "Shaders/brdf.frag", "brdf");

	loadModel("../assets/Sponza/Sponza.gltf", "Sponza");
	loadEnvironmentMap("default", "../assets/8ksky.hdr", equirectCubemapShader, irradianceShader, prefilterShader, brdfShader);
	

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
	loadShader("Shaders/luminance.comp", "Luminance");
	loadShader("Shaders/adaptation.comp", "Adaptation");
	loadShader("Shaders/ssr.vert", "Shaders/ssr.frag", "SSR");
}

void GraphicsManager::clear()
{
	mShaderMap.clear();
	mTextureMap.clear();
	mMaterialMap.clear();
	mModelMap.clear();
	mEnvironmentMap.clear();
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath, std::string name)
{
	// Load and compile shader
	auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
	if (shader)
	{
		mShaderMap[name] = shader;
	}
	else
	{
		std::cerr << "GraphicsManager::loadShader: Failed to load shader '" << name << "' from " << vertexPath << ", " << fragmentPath << std::endl;
	}
	return shader;
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, std::string name)
{
	// Load and compile shader
	auto shader = std::make_shared<Shader>(vertexPath, fragmentPath, geometryPath);
	if (shader)
	{
		mShaderMap.emplace(std::move(name), shader);
	}
	else
	{
		std::cerr << "GraphicsManager::loadShader: Failed to load shader '" << name << "' from " << vertexPath << ", " << fragmentPath << ", " << geometryPath << std::endl;
	}
	return shader;
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& computePath, std::string name)
{
	// Load and compile shader
	auto shader = std::make_shared<Shader>(computePath);
	if (shader)
	{
		mShaderMap.emplace(std::move(name), shader);
	}
	else
	{
		std::cerr << "GraphicsManager::loadShader: Failed to load compute shader '" << name << "' from " << computePath << std::endl;
	}
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
		std::cerr << "GraphicsManager::getShader: Shader not found: '" << name << "'" << std::endl;
		return nullptr;
	}
}

// Texture management

std::shared_ptr<Texture> GraphicsManager::loadTexture(const std::string& path,
	aiTextureType type,
	const aiScene* scene)
{
	// We'll build a unique key for texture_map_.
	std::string uniqueKey;
	bool isEmbedded = (!path.empty() && path[0] == '*');

	if (isEmbedded)
	{
		// Example: embedded textures often show up as "*0", "*1", etc.
		unsigned int textureIndex = 0;
		try {
			textureIndex = std::stoi(path.substr(1)); // e.g. from "*0"
		} catch (const std::invalid_argument& ia) {
			std::cerr << "GraphicsManager::loadTexture: Invalid embedded texture index format: " << path << std::endl;
			return nullptr;
		} catch (const std::out_of_range& oor) {
			 std::cerr << "GraphicsManager::loadTexture: Embedded texture index out of range: " << path << std::endl;
			return nullptr;
		}


		// Construct a unique key: e.g., "Embedded_140535221312672_*0"
		uintptr_t scenePtrVal = reinterpret_cast<uintptr_t>(scene);
		uniqueKey = "Embedded_" + std::to_string(scenePtrVal) + "_" + path;

		// Check cache first
		if (auto it = mTextureMap.find(uniqueKey); it != mTextureMap.end())
		{
			return it->second; // Already loaded
		}

		// Validate scene and index before accessing
		if (!scene || textureIndex >= scene->mNumTextures) {
			std::cerr << "GraphicsManager::loadTexture: Invalid scene or texture index for embedded texture: " << path << std::endl;
			return nullptr;
		}

		// Load embedded texture from Assimp
		aiTexture* aiTex = scene->mTextures[textureIndex];
		auto texture = std::make_shared<Texture>(aiTex);
		if (texture) // Check if texture pointer is valid (basic check)
		{
			mTextureMap.emplace(uniqueKey, texture); // Use emplace
		}
		else
		{
			std::cerr << "GraphicsManager::loadTexture: Failed to load embedded texture at index: " << textureIndex << " (key: " << uniqueKey << ")" << std::endl;
			return nullptr; // Return null if loading failed
		}
		return texture;
	}
	else // External file path
	{
		std::filesystem::path fsPath(path);
		std::string canonicalPathStr;

		// Use canonical path if the file exists for uniqueness
		std::error_code ec;
		if (std::filesystem::exists(fsPath, ec) && !ec)
		{
			canonicalPathStr = std::filesystem::canonical(fsPath, ec).string();
			if (ec) {
				std::cerr << "GraphicsManager::loadTexture: Error getting canonical path for '" << path << "': " << ec.message() << ". Using original path as key." << std::endl;
				canonicalPathStr = path; // Fallback to original path
			}
		}
		else {
			// File doesn't exist or error checking existence, use the provided path directly.
			// Assimp might still find it relative to the model file.
			canonicalPathStr = path;
			if(ec) { // Log existence check error if any
				std::cerr << "GraphicsManager::loadTexture: Error checking existence for '" << path << "': " << ec.message() << ". Using original path as key." << std::endl;
			}
		}

		uniqueKey = canonicalPathStr; // Use canonical (or original if failed/not found) path as key

		// Check cache first
		if (auto it = mTextureMap.find(uniqueKey); it != mTextureMap.end())
		{
			return it->second;
		}

		// Load from file
		auto texture = std::make_shared<Texture>(uniqueKey); // Load using the determined key (path)
		if (texture) // Check if texture pointer is valid (basic check)
		{
			mTextureMap.emplace(uniqueKey, texture); // Use emplace
		}
		else
		{
			std::cerr << "GraphicsManager::loadTexture: Failed to load texture from file: '" << uniqueKey << "'" << std::endl;
			return nullptr; // Return null if loading failed
		}
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
		// Attempt lookup by canonical path as a fallback (potential performance hit)
		std::error_code ec;
		if (std::filesystem::exists(path, ec) && !ec) {
			std::string canonicalPathStr = std::filesystem::canonical(path, ec).string();
			if (!ec) {
				it = mTextureMap.find(canonicalPathStr);
				if (it != mTextureMap.end()) {
					return it->second;
				}
			}
		}
		// Log if still not found
		std::cerr << "GraphicsManager::getTexture: Texture not found by path or canonical path: '" << path << "'" << std::endl;
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
		std::cerr << "GraphicsManager::getMaterial: Material not found: '" << name << "'" << std::endl;
		return nullptr;
	}
}

// Mesh management

// Model and Scene management


std::shared_ptr<Model> GraphicsManager::getModel(const std::string& name)
{
	auto it = mModelMap.find(name);
	if (it != mModelMap.end())
	{
		return it->second;
	}
	else
	{
		std::cerr << "GraphicsManager::getModel: Model not found: '" << name << "'" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<Model> GraphicsManager::loadModel(const std::string& path, std::string name)
{
	// Check if model already loaded
	auto it = mModelMap.find(name);
	if (it != mModelMap.end())
	{
		std::cout << "GraphicsManager::loadModel: Model '" << name << "' already loaded. Returning cached version." << std::endl; // Info level log
		return it->second;
	}

	// Load model using Assimp
	Assimp::Importer importer;
	// Consider adding aiProcess_JoinIdenticalVertices for optimization
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_ValidateDataStructure |
		aiProcess_EmbedTextures
		// | aiProcess_JoinIdenticalVertices // Optional optimization
		);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "GraphicsManager::loadModel: Assimp error loading '" << path << "': " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

    // Use std::filesystem for robust path handling
    std::filesystem::path modelPath(path);
    std::string directory = modelPath.parent_path().string();

	// Process the root node recursively
	std::vector<MeshInstance> meshInstances;
	processNode(scene->mRootNode, scene, directory, glm::mat4(1.0f), meshInstances);

	if (meshInstances.empty()) {
		std::cerr << "GraphicsManager::loadModel: No meshes processed for model '" << name << "' from path '" << path << "'. Check model file or processing logic." << std::endl;
		// Optionally return nullptr or an empty model depending on desired behavior
	}

	// Create the model with mesh instances
	auto model = std::make_shared<Model>(std::move(meshInstances)); // Move mesh instances into the model
	model->setName(name); // Set the name on the model object itself
	mModelMap.emplace(std::move(name), model); // Move name into the map key

	std::cout << "GraphicsManager::loadModel: Successfully loaded model '" << model->getName() << "' from '" << path << "'" << std::endl; // Info log
	return model;
}



std::shared_ptr<Mesh> GraphicsManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
{
	if (!mesh) {
		std::cerr << "GraphicsManager::processMesh: Received null aiMesh pointer." << std::endl;
		return nullptr;
	}

	// Process vertices
	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices); // Pre-allocate memory
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{}; // Value initialize

		// Positions (Required)
		vertex.position = glm::vec3(mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z);

		// Normals
		if (mesh->HasNormals())
		{
			vertex.normal = glm::vec3(mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z);
		}
		else {
			// Consider logging a warning if normals are expected but missing
			// std::cerr << "GraphicsManager::processMesh: Warning: Mesh '" << mesh->mName.C_Str() << "' missing normals." << std::endl;
		}

		// Texture Coordinates
		if (mesh->HasTextureCoords(0)) // Assumes only one UV channel
		{
			vertex.texture_coordinates = glm::vec2(mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y);
		}
		// else: texture_coordinates remains (0.0f, 0.0f) due to value initialization

		// Tangents and Bitangents
		if (mesh->HasTangentsAndBitangents())
		{
			vertex.tangent = glm::vec3(mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z);

			vertex.bitangent = glm::vec3(mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z);
		}
		// else: tangent/bitangent remain zero vectors

		vertices.push_back(vertex);
	}

	// Process indices
	std::vector<unsigned int> indices;
	indices.reserve(mesh->mNumFaces * 3); // Estimate index count (assuming triangles)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3) {
			std::cerr << "GraphicsManager::processMesh: Warning: Non-triangular face encountered in mesh '" << mesh->mName.C_Str() << "'. Index count: " << face.mNumIndices << ". Skipping face." << std::endl;
			continue; // Skip non-triangles if aiProcess_Triangulate didn't handle it
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
		aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];
		material = loadMaterial(ai_material, directory, scene);
		if (!material) {
			std::cerr << "GraphicsManager::processMesh: Warning: Failed to load material for mesh '" << mesh->mName.C_Str() << "' (Material Index: " << mesh->mMaterialIndex << "). Mesh will have no material." << std::endl;
			// Assign a default material or handle appropriately
		}
	} else {
		std::cerr << "GraphicsManager::processMesh: Warning: Mesh '" << mesh->mName.C_Str() << "' has invalid material index (" << mesh->mMaterialIndex << ") or scene has no materials." << std::endl;
		// Assign a default material or handle appropriately
	}


	// Create the mesh - No longer stored in GraphicsManager's map
	auto new_mesh = std::make_shared<Mesh>(std::move(vertices), std::move(indices), material);

	// Set mesh name (optional, useful for debugging)
	std::string mesh_name = mesh->mName.C_Str();
	if (mesh_name.empty())
	{
		// Generate a placeholder name if Assimp didn't provide one
		mesh_name = "unnamed_mesh_" + std::to_string(reinterpret_cast<uintptr_t>(mesh));
	}
	new_mesh->setName(mesh_name);


	return new_mesh;
}

std::shared_ptr<Material> GraphicsManager::loadMaterial(aiMaterial* mat, const std::string& directory, const aiScene* scene)
{
	if (!mat) {
		std::cerr << "GraphicsManager::loadMaterial: Received null aiMaterial pointer." << std::endl;
		return nullptr;
	}
	// Get material name
	aiString name;
	mat->Get(AI_MATKEY_NAME, name);
	std::string material_name_str = name.C_Str(); // Convert aiString to std::string

	// Use pointer address for uniqueness if name is empty, as size() is unreliable now.
	// Or simply ensure all materials have names in the asset pipeline.
	if (material_name_str.empty())
	{
		uintptr_t matPtrVal = reinterpret_cast<uintptr_t>(mat);
		material_name_str = "material_" + std::to_string(matPtrVal);
		// Log a warning about the unnamed material
        std::cout << "GraphicsManager::loadMaterial: Warning: Material has no name. Generating unique name: " << material_name_str << std::endl;
	}

	// Check if material already exists in the cache
	auto it = mMaterialMap.find(material_name_str);
	if (it != mMaterialMap.end())
	{
		return it->second;
	}

	// Create new material
	auto material = std::make_shared<Material>();
	// material->setShader(getShader("PBR")); // Consider making shader assignment more flexible or data-driven

	// --- Extract Material Properties ---
    // Removed helper lambda - handle properties directly

	aiColor4D color; // Use aiColor4D for aiGetMaterialColor
	// Base color/albedo (PBR primary) - Use aiGetMaterialColor
	if (aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &color) == AI_SUCCESS) {
		material->setAlbedo(glm::vec4(color.r, color.g, color.b, color.a));
	} // Fallback to legacy diffuse (check if BASE_COLOR wasn't found) - Use aiGetMaterialColor
	else if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
		material->setAlbedo(glm::vec4(color.r, color.g, color.b, color.a));
		std::cout << "GraphicsManager::loadMaterial: Info: Using legacy diffuse color for albedo in material '" << material_name_str << "'" << std::endl;
	} else {
        // Default albedo if neither is found
        material->setAlbedo(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

	// --- Use aiGetMaterialProperty for float values ---
	const aiMaterialProperty* prop = nullptr;

	// Metallic factor (PBR)
    float metallic = 0.0f; // Default value
	if (aiGetMaterialProperty(mat, AI_MATKEY_METALLIC_FACTOR, &prop) == AI_SUCCESS && prop && prop->mType == aiPTI_Float && prop->mDataLength >= sizeof(float)) {
        metallic = *reinterpret_cast<float*>(prop->mData);
    }
	material->setMetallic(metallic);
    prop = nullptr; // Reset prop for next property

	// Roughness factor (PBR)
    float roughness = 0.5f; // Default value
    if (aiGetMaterialProperty(mat, AI_MATKEY_ROUGHNESS_FACTOR, &prop) == AI_SUCCESS && prop && prop->mType == aiPTI_Float && prop->mDataLength >= sizeof(float)) {
        roughness = *reinterpret_cast<float*>(prop->mData);
    }
	material->setRoughness(roughness);
    prop = nullptr;

	// Opacity (Common)
    float opacity = 1.0f; // Default value
    if (aiGetMaterialProperty(mat, AI_MATKEY_OPACITY, &prop) == AI_SUCCESS && prop && prop->mType == aiPTI_Float && prop->mDataLength >= sizeof(float)) {
        opacity = *reinterpret_cast<float*>(prop->mData);
    }
	material->setOpacity(opacity);
    prop = nullptr;

	// Emissive properties (Common) - Use aiGetMaterialColor
	aiColor4D emissiveColor4D; // aiGetMaterialColor outputs aiColor4D
	if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor4D) == AI_SUCCESS) {
		// Extract RGB from aiColor4D for setEmissiveColor (which takes vec3)
		material->setEmissiveColor(glm::vec3(emissiveColor4D.r, emissiveColor4D.g, emissiveColor4D.b));
	} else {
        material->setEmissiveColor(glm::vec3(0.0f)); // Set default if Get fails
    }

    float emissiveIntensity = 0.0f; // Default
    if (aiGetMaterialProperty(mat, AI_MATKEY_EMISSIVE_INTENSITY, &prop) == AI_SUCCESS && prop && prop->mType == aiPTI_Float && prop->mDataLength >= sizeof(float)) {
        emissiveIntensity = *reinterpret_cast<float*>(prop->mData);
    }
	material->setEmissiveIntensity(emissiveIntensity);
    prop = nullptr;


	// Displacement/Bump Scale (Common)
    float displacementScale = 0.1f; // Default
    if (aiGetMaterialProperty(mat, AI_MATKEY_BUMPSCALING, &prop) == AI_SUCCESS && prop && prop->mType == aiPTI_Float && prop->mDataLength >= sizeof(float)) {
        displacementScale = *reinterpret_cast<float*>(prop->mData);
    }
	material->setDisplacementScale(displacementScale);
    prop = nullptr;

	// Reflectivity (Legacy, less common in PBR)
    float reflectivity = 0.0f; // Default
    if (aiGetMaterialProperty(mat, AI_MATKEY_REFLECTIVITY, &prop) == AI_SUCCESS && prop && prop->mType == aiPTI_Float && prop->mDataLength >= sizeof(float)) {
        reflectivity = *reinterpret_cast<float*>(prop->mData);
    }
	material->setReflectivity(reflectivity);
    prop = nullptr;

	// --- Load Textures ---
	// Define a helper lambda for texture loading to reduce repetition
    auto loadTextureType = [&](aiTextureType texType) -> std::shared_ptr<Texture> {
        auto textures = loadMaterialTextures(mat, texType, directory, scene);
        if (!textures.empty()) {
            if (textures.size() > 1) {
                std::cout << "GraphicsManager::loadMaterial: Warning: Material '" << material_name_str
                          << "' has multiple textures of type " << texType << ". Using the first one." << std::endl;
            }
            return textures[0];
        }
        return nullptr;
    };

	material->setAlbedoTexture(loadTextureType(aiTextureType_BASE_COLOR));
	// Fallback for albedo if BASE_COLOR texture not found
    if (!material->getAlbedoTexture()) {
        material->setAlbedoTexture(loadTextureType(aiTextureType_DIFFUSE));
         if (material->getAlbedoTexture()) {
             std::cout << "GraphicsManager::loadMaterial: Info: Using legacy diffuse texture for albedo in material '" << material_name_str << "'" << std::endl;
         }
    }

	material->setNormalTexture(loadTextureType(aiTextureType_NORMALS));
    // Some exporters might use height maps for normals
    if (!material->getNormalTexture()) {
        material->setNormalTexture(loadTextureType(aiTextureType_HEIGHT));
         if (material->getNormalTexture()) {
             std::cout << "GraphicsManager::loadMaterial: Info: Using height texture as normal map in material '" << material_name_str << "'" << std::endl;
         }
    }

	material->setMetallicTexture(loadTextureType(aiTextureType_METALNESS));
	material->setRoughnessTexture(loadTextureType(aiTextureType_DIFFUSE_ROUGHNESS));
	material->setAOTexture(loadTextureType(aiTextureType_AMBIENT_OCCLUSION)); // Often stored in LIGHTMAP channel in glTF
    if (!material->getAOTexture()) {
         material->setAOTexture(loadTextureType(aiTextureType_LIGHTMAP));
         if (material->getAOTexture()) {
             std::cout << "GraphicsManager::loadMaterial: Info: Using lightmap texture as AO map in material '" << material_name_str << "'" << std::endl;
         }
    }


	material->setEmissiveTexture(loadTextureType(aiTextureType_EMISSIVE)); // Check AI_MATKEY_USE_EMISSIVE_MAP?
    if (!material->getEmissiveTexture()) {
         material->setEmissiveTexture(loadTextureType(aiTextureType_EMISSION_COLOR));
    }


	material->setDisplacementTexture(loadTextureType(aiTextureType_DISPLACEMENT));

	// Handle combined metallic-roughness texture (common in glTF packed as ORM or similar)
	// Assimp maps this to aiTextureType_UNKNOWN for glTF. Need specific channel checks if loading raw glTF data.
    // For standard Assimp import, metallic and roughness maps are usually separate if available.
	auto metalRoughTexture = loadTextureType(aiTextureType_UNKNOWN); // Check if Assimp loads packed textures this way
    if (metalRoughTexture) {
        // Heuristic: If separate metallic/roughness maps weren't loaded, assume this is packed.
        if (!material->getMetallicTexture() && !material->getRoughnessTexture()) {
            material->setMetalRoughTexture(metalRoughTexture);
             std::cout << "GraphicsManager::loadMaterial: Info: Using UNKNOWN texture type as potential packed Metal/Rough map for material '" << material_name_str << "'" << std::endl;
        } else {
            std::cout << "GraphicsManager::loadMaterial: Warning: Found UNKNOWN texture type but also separate Metal/Rough maps for material '" << material_name_str << "'. Ignoring UNKNOWN texture." << std::endl;
        }
    }


	// Store in material map
	material->setName(material_name_str); // Set name on the material object
	mMaterialMap.emplace(std::move(material_name_str), material); // Move name string into map

	return material;
}



std::vector<std::shared_ptr<Texture>> GraphicsManager::loadMaterialTextures(
	aiMaterial* mat,
	aiTextureType type,
	const std::string& directory,
	const aiScene* scene)
{
	std::vector<std::shared_ptr<Texture>> textures;
	unsigned int textureCount = mat->GetTextureCount(type);
    textures.reserve(textureCount); // Pre-allocate space

	for (unsigned int i = 0; i < textureCount; i++)
	{
		aiString ai_str;
		if (mat->GetTexture(type, i, &ai_str) != AI_SUCCESS) {
            std::cerr << "GraphicsManager::loadMaterialTextures: Failed to get texture path for type " << type << " at index " << i << std::endl;
            continue; // Skip this texture
        }

		std::string texture_path_str = ai_str.C_Str();
		std::string full_path_or_key; // Will hold either the embedded key or the full file path

		// Determine if the texture is embedded or external
		bool isEmbedded = (!texture_path_str.empty() && texture_path_str[0] == '*');

		if (isEmbedded)
		{
			// Embedded texture: Use the texture path string directly as the *initial* lookup key part
			// The actual unique key generation happens inside loadTexture
			full_path_or_key = texture_path_str;
		}
		else
		{
			// External texture: Construct the full path relative to the model directory
			std::filesystem::path dirPath(directory);
			std::filesystem::path texPath(texture_path_str);
            std::filesystem::path combinedPath;

            // Handle cases where texture_path_str might be absolute already
            if (texPath.is_absolute()) {
                combinedPath = texPath;
            } else {
			    combinedPath = dirPath / texPath;
            }

			// Normalize the path for consistency (optional but recommended)
			// full_path_or_key = std::filesystem::weakly_canonical(combinedPath).string(); // Use weakly_canonical to handle non-existent paths gracefully
            full_path_or_key = combinedPath.lexically_normal().string(); // Simpler normalization

		}

        // Use the central loadTexture function which handles caching and path resolution
		auto texture = loadTexture(full_path_or_key, type, scene);

		if (texture) // Check if loadTexture returned a valid texture
		{
            // Apply specific sampler parameters based on texture type AFTER loading/retrieval
            // This ensures parameters are set even if the texture was cached.
            if (type == aiTextureType_NORMALS || type == aiTextureType_HEIGHT || type == aiTextureType_DISPLACEMENT)
            {
                texture->setNormalSamplerParameters(); // Assuming this sets things like clamp to edge, etc.
            }
            // Add more parameter settings for other types if needed (e.g., linear vs nearest for certain maps)

			textures.push_back(texture);
		}
		else
		{
			// loadTexture already prints an error, but we can add context
			std::cerr << "GraphicsManager::loadMaterialTextures: Failed attempt to load texture (type " << type << ", index " << i << ") with path/key: '" << full_path_or_key << "'" << std::endl;
		}
	}
	return textures; // Return vector (potentially empty)
}

std::shared_ptr<EnvironmentMap> GraphicsManager::loadEnvironmentMap(std::string name, const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader)
{
	auto it = mEnvironmentMap.find(name);
	if (it != mEnvironmentMap.end())
	{
        std::cout << "GraphicsManager::loadEnvironmentMap: Environment map '" << name << "' already loaded. Returning cached version." << std::endl;
		return it->second;
	}

    // Validate required shaders
    if (!equirectangularToCubemapShader || !irradianceShader || !prefilterShader || !brdfShader) {
        std::cerr << "GraphicsManager::loadEnvironmentMap: Error loading environment map '" << name << "'. One or more required shaders are missing." << std::endl;
        return nullptr;
    }

	auto environmentMap = std::make_shared<EnvironmentMap>(hdrPath, equirectangularToCubemapShader, irradianceShader, prefilterShader, brdfShader);

    // Check if the environment map loaded successfully (assuming EnvironmentMap has a way to check, e.g., isLoaded())
    // if (!environmentMap || !environmentMap->isLoaded()) { // Example check
    //     std::cerr << "GraphicsManager::loadEnvironmentMap: Failed to create or load environment map '" << name << "' from HDR path '" << hdrPath << "'." << std::endl;
    //     return nullptr;
    // }

	mEnvironmentMap.emplace(std::move(name), environmentMap); // Move name into map
	std::cout << "GraphicsManager::loadEnvironmentMap: Successfully loaded environment map '" << name << "' from '" << hdrPath << "'" << std::endl;
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
		std::cerr << "GraphicsManager::getEnvironmentMap: EnvironmentMap not found: '" << name << "'" << std::endl;
		return nullptr;
	}
}

void GraphicsManager::processNode(aiNode* node, const aiScene* scene, const std::string& directory, const glm::mat4& parentTransform, std::vector<MeshInstance>& meshInstances)
{
    if (!node || !scene) {
        std::cerr << "GraphicsManager::processNode: Received null node or scene pointer." << std::endl;
        return;
    }
	// Convert aiMatrix4x4 to glm::mat4 and combine with parent transform
	glm::mat4 nodeTransform = parentTransform * aiMatrixToGlm(node->mTransformation);

	// Process all the node's meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
        unsigned int meshIndex = node->mMeshes[i];
        if (meshIndex >= scene->mNumMeshes) {
            std::cerr << "GraphicsManager::processNode: Node '" << node->mName.C_Str() << "' contains invalid mesh index: " << meshIndex << ". Skipping mesh." << std::endl;
            continue;
        }

		aiMesh* ai_mesh = scene->mMeshes[meshIndex];
		auto mesh = processMesh(ai_mesh, scene, directory); // Process mesh directly
		if (mesh) // Check if mesh processing was successful
		{
			MeshInstance meshInstance;
			meshInstance.mesh = mesh; // Assign the returned mesh shared_ptr
			meshInstance.localTransform = nodeTransform; // Store the combined transform for this instance
			meshInstances.push_back(std::move(meshInstance)); // Move the instance into the vector
		}
        else {
            std::cerr << "GraphicsManager::processNode: Failed to process mesh at index " << meshIndex << " in node '" << node->mName.C_Str() << "'. Skipping mesh instance." << std::endl;
        }
	}

	// Recursively process each child node
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, directory, nodeTransform, meshInstances); // Pass nodeTransform (this node's world transform) as parentTransform to children
	}
}

glm::mat4 GraphicsManager::aiMatrixToGlm(const aiMatrix4x4& aiMat)
{
	glm::mat4 mat;
	// Direct mapping (assuming glm::mat4 is column-major like OpenGL)
	// aiMatrix4x4 is row-major
	mat[0][0] = aiMat.a1; mat[1][0] = aiMat.a2; mat[2][0] = aiMat.a3; mat[3][0] = aiMat.a4;
	mat[0][1] = aiMat.b1; mat[1][1] = aiMat.b2; mat[2][1] = aiMat.b3; mat[3][1] = aiMat.b4;
	mat[0][2] = aiMat.c1; mat[1][2] = aiMat.c2; mat[2][2] = aiMat.c3; mat[3][2] = aiMat.c4;
	mat[0][3] = aiMat.d1; mat[1][3] = aiMat.d2; mat[2][3] = aiMat.d3; mat[3][3] = aiMat.d4;
	return mat;
}