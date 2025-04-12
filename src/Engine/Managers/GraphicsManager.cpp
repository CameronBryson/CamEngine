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
	mMeshMap.clear();
	mModelMap.clear();
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name)
{
	// Load and compile shader
	auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
	if (shader)
	{
		//shader_map_.emplace(name, shader);
		mShaderMap[name] = shader;
	}
	else
	{
		std::cerr << "Failed to load shader: " << name << std::endl;
	}
	return shader;
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& name)
{
	// Load and compile shader
	auto shader = std::make_shared<Shader>(vertexPath, fragmentPath, geometryPath);
	if (shader)
	{
		mShaderMap.emplace(name, shader);
	}
	else
	{
		std::cerr << "Failed to load shader: " << name << std::endl;
	}
	return shader;
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& computePath, const std::string& name)
{
	// Load and compile shader
	auto shader = std::make_shared<Shader>(computePath);
	if (shader)
	{
		mShaderMap.emplace(name, shader);
	}
	else
	{
		std::cerr << "Failed to load shader: " << name << std::endl;
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
		std::cerr << "Shader not found: " << name << std::endl;
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

	// If this is an embedded texture (path starts with '*'):
	if (!path.empty() && path[0] == '*')
	{
		// Example: embedded textures often show up as "*0", "*1", etc.
		// We'll combine the pointer to the aiScene plus the index for uniqueness.
		// This ensures two scenes each having "*0" won't collide.
		unsigned int textureIndex = std::stoi(path.substr(1)); // e.g. from "*0"

		// Construct a unique key: e.g., "Embedded_140535221312672_*0"
		// scene is cast to a numeric value to help differentiate.
		// You can also store a "model name" if you have it.
		uintptr_t scenePtr = reinterpret_cast<uintptr_t>(scene);
		uniqueKey = "Embedded_" + std::to_string(scenePtr) + "_" + path;

		// Check if we already have it
		if (auto it = mTextureMap.find(uniqueKey); it != mTextureMap.end())
		{
			return it->second; // Already loaded
		}

		// Actually load the embedded texture from Assimp
		unsigned int texIndex = textureIndex;
		aiTexture* aiTex = scene->mTextures[texIndex];
		auto texture = std::make_shared<Texture>(aiTex);
		if (texture)
		{
			mTextureMap.emplace(uniqueKey, texture);
		}
		else
		{
			std::cerr << "Failed to load embedded texture at index: " << texIndex << std::endl;
		}
		return texture;
	}
	else
	{
		// External file. We continue with your existing logic of canonical paths.

		// Start with the raw input path.
		std::string full_path = path;

		// If the file actually exists, get its canonical path.
		if (std::filesystem::exists(path))
		{
			full_path = std::filesystem::canonical(path).string();
		}

		// Optionally also incorporate the texture type in the key if you want:
		// e.g. full_path += "#type_" + std::to_string(type);

		// Now this is your unique key for external textures.
		uniqueKey = full_path;

		// Check if we already have this external texture loaded
		if (auto it = mTextureMap.find(uniqueKey); it != mTextureMap.end())
		{
			return it->second;
		}

		// Otherwise, load from file
		auto texture = std::make_shared<Texture>(full_path);
		if (texture)
		{
			mTextureMap.emplace(uniqueKey, texture);
		}
		else
		{
			std::cerr << "Failed to load texture: " << full_path << std::endl;
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
		std::cerr << "Texture not found: " << path << std::endl;
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
		std::cerr << "Material not found: " << name << std::endl;
		return nullptr;
	}
}

// Mesh management

std::shared_ptr<Mesh> GraphicsManager::createMesh(const std::string& name,
	const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices,
	const std::shared_ptr<Material>& material)
{
	// Check if mesh already exists
	auto it = mMeshMap.find(name);
	if (it != mMeshMap.end())
	{
		return it->second;
	}

	// Create new mesh
	auto mesh = std::make_shared<Mesh>(vertices, indices, material);
	mesh->setName(name);
	mMeshMap.emplace(name, mesh);

	return mesh;
}

std::shared_ptr<Mesh> GraphicsManager::getMesh(const std::string& name)
{
	auto it = mMeshMap.find(name);
	if (it != mMeshMap.end())
	{
		return it->second;
	}
	else
	{
		std::cerr << "Mesh not found: " << name << std::endl;
		return nullptr;
	}
}

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
		std::cerr << "Model not found: " << name << std::endl;
		return nullptr;
	}
}

std::shared_ptr<Model> GraphicsManager::loadModel(const std::string& path, const std::string& name)
{
	// Check if model already loaded
	auto it = mModelMap.find(name);
	if (it != mModelMap.end())
	{
		return it->second;
	}

	// Load model using Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_ValidateDataStructure |
		aiProcess_EmbedTextures
		);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	std::string directory = std::filesystem::path(path).parent_path().string();

	// Process the root node recursively
	std::vector<MeshInstance> meshInstances;
	processNode(scene->mRootNode, scene, directory, glm::mat4(1.0f), meshInstances);

	// Create the model with mesh instances
	auto model = std::make_shared<Model>(meshInstances);
	model->setName(name);
	mModelMap.emplace(name, model);

	return model;
}



std::shared_ptr<Mesh> GraphicsManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
{
	// Generate unique mesh name
	std::string mesh_name = mesh->mName.C_Str();
	if (mesh_name.empty())
	{
		mesh_name = "mesh_" + std::to_string(mMeshMap.size());
	}

	// Check if mesh already exists
	auto it = mMeshMap.find(mesh_name);
	if (it != mMeshMap.end())
	{
		return it->second;
	}

	// Process vertices
	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// Positions
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

		// Texture Coordinates
		if (mesh->HasTextureCoords(0))
		{
			vertex.texture_coordinates = glm::vec2(mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.texture_coordinates = glm::vec2(0.0f, 0.0f);
		}

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

		vertices.push_back(vertex);
	}

	// Process indices
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];
	auto material = loadMaterial(ai_material, directory, scene);

	// Create the mesh
	auto new_mesh = createMesh(mesh_name, vertices, indices, material);

	return new_mesh;
}

std::shared_ptr<Material> GraphicsManager::loadMaterial(aiMaterial* mat, const std::string& directory, const aiScene* scene)
{
	// Get material name
	aiString name;
	mat->Get(AI_MATKEY_NAME, name);
	std::string material_name = name.C_Str();
	if (material_name.empty())
	{
		material_name = "material_" + std::to_string(mMaterialMap.size());
	}

	// Check if material already exists
	auto it = mMaterialMap.find(material_name);
	if (it != mMaterialMap.end())
	{
		return it->second;
	}

	// Create new material
	auto material = std::make_shared<Material>();
	//material->setShader(getShader("PBR"));

	// Base color/albedo
	aiColor4D baseColor;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_BASE_COLOR, baseColor)) {
		material->setAlbedo(glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a));
	}
	else {
		// Fallback to legacy diffuse color
		aiColor4D diffuseColor;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)) {
			material->setAlbedo(glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
		}
	}

	// Metallic factor
	float metallic = 0.0f;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic)) {
		material->setMetallic(metallic);
	}

	// Roughness factor
	float roughness = 0.5f;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness)) {
		material->setRoughness(roughness);
	}


	// Opacity
	float opacity = 1.0f;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_OPACITY, opacity)) {
		material->setOpacity(opacity);
	}

	// Emissive properties
	aiColor3D emissiveColor;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor)) {
		material->setEmissiveColor(glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b));
	}

	float emissiveIntensity = 0.0f;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissiveIntensity)) {
		material->setEmissiveIntensity(emissiveIntensity);
	}

	// Displacement
	float displacementScale = 0.1f;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_BUMPSCALING, displacementScale)) {
		material->setDisplacementScale(displacementScale);
	}

	float reflectivity = 0.2f;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_REFLECTIVITY, reflectivity))
	{
		material->setReflectivity(reflectivity);
	}

	// Load textures
	auto albedoTextures = loadMaterialTextures(mat, aiTextureType_BASE_COLOR, directory, scene);
	if (!albedoTextures.empty()) material->setAlbedoTexture(albedoTextures[0]);

	auto normalTextures = loadMaterialTextures(mat, aiTextureType_NORMALS, directory, scene);
	if (!normalTextures.empty())material->setNormalTexture(normalTextures[0]);


	auto metallicTextures = loadMaterialTextures(mat, aiTextureType_METALNESS, directory, scene);
	if (!metallicTextures.empty()) material->setMetallicTexture(metallicTextures[0]);

	auto roughnessTextures = loadMaterialTextures(mat, aiTextureType_DIFFUSE_ROUGHNESS, directory, scene);
	if (!roughnessTextures.empty()) material->setRoughnessTexture(roughnessTextures[0]);

	auto aoTextures = loadMaterialTextures(mat, aiTextureType_AMBIENT_OCCLUSION, directory, scene);
	if (!aoTextures.empty()) material->setAOTexture(aoTextures[0]);

	auto emissiveTextures = loadMaterialTextures(mat, aiTextureType_EMISSION_COLOR, directory, scene);
	if (!emissiveTextures.empty()) material->setEmissiveTexture(emissiveTextures[0]);

	auto displacementTextures = loadMaterialTextures(mat, aiTextureType_DISPLACEMENT, directory, scene);
	if (!displacementTextures.empty()) material->setDisplacementTexture(displacementTextures[0]);

	// Handle combined metallic-roughness texture (common in glTF)
	auto metalRoughTextures = loadMaterialTextures(mat, aiTextureType_UNKNOWN, directory, scene);
	if (!metalRoughTextures.empty()) material->setMetalRoughTexture(metalRoughTextures[0]);

	// Store in material map
	material->setName(material_name);
	mMaterialMap.emplace(material_name, material);

	return material;
}



std::vector<std::shared_ptr<Texture>> GraphicsManager::loadMaterialTextures(
	aiMaterial* mat,
	aiTextureType type,
	const std::string& directory,
	const aiScene* scene)
{
	std::vector<std::shared_ptr<Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{

		aiString ai_str;
		mat->GetTexture(type, i, &ai_str);

		std::string texture_path = ai_str.C_Str();
		std::string full_path;

		// Determine if the texture is embedded
		if (!texture_path.empty() && texture_path[0] == '*')
		{
			// Embedded texture: Use the texture path as-is
			full_path = texture_path;
		}
		else
		{
			// External texture: Prepend the directory path
			full_path = directory + "/" + texture_path;
		}

		auto texture = loadTexture(full_path, type, scene);
		// Apply texture parameters based on type and retrieved mapModes
		if (type == aiTextureType_NORMALS || type == aiTextureType_HEIGHT) 
		{
			// Normal maps need special handling
			texture->setNormalSamplerParameters();
		}
		if (texture)
		{
			textures.push_back(texture);
		}
		else
		{
			std::cerr << "Failed to load texture: " << full_path << std::endl;
		}
	}
	return textures;
}

std::shared_ptr<EnvironmentMap> GraphicsManager::loadEnvironmentMap(const std::string& name, const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader)
{
	auto it = mEnvironmentMap.find(name);
	if (it != mEnvironmentMap.end())
	{
		return it->second;
	}
	auto environmentMap = std::make_shared<EnvironmentMap>(hdrPath, equirectangularToCubemapShader, irradianceShader, prefilterShader, brdfShader);
	mEnvironmentMap.emplace(name, environmentMap);
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
		std::cerr << "EnvironmentMap not found" << name << std::endl;
		return nullptr;
	}
}

void GraphicsManager::processNode(aiNode* node, const aiScene* scene, const std::string& directory, const glm::mat4& parentTransform, std::vector<MeshInstance>& meshInstances)
{
	// Convert aiMatrix4x4 to glm::mat4 and combine with parent transform
	glm::mat4 nodeTransform = parentTransform * aiMatrixToGlm(node->mTransformation);

	// Process all the node's meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		auto mesh = processMesh(ai_mesh, scene, directory);
		if (mesh)
		{
			MeshInstance meshInstance(mesh,nodeTransform);
			meshInstances.push_back(meshInstance);
		}
	}

	// Recursively process each child node
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, directory, nodeTransform, meshInstances);
	}
}

glm::mat4 GraphicsManager::aiMatrixToGlm(const aiMatrix4x4& aiMat)
{
	glm::mat4 mat;
	mat[0][0] = aiMat.a1; mat[0][1] = aiMat.b1; mat[0][2] = aiMat.c1; mat[0][3] = aiMat.d1;
	mat[1][0] = aiMat.a2; mat[1][1] = aiMat.b2; mat[1][2] = aiMat.c2; mat[1][3] = aiMat.d2;
	mat[2][0] = aiMat.a3; mat[2][1] = aiMat.b3; mat[2][2] = aiMat.c3; mat[2][3] = aiMat.d3;
	mat[3][0] = aiMat.a4; mat[3][1] = aiMat.b4; mat[3][2] = aiMat.c4; mat[3][3] = aiMat.d4;
	return mat;
}