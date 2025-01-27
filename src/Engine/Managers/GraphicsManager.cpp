#include "pch.hpp"
#include "EngineUtil.hpp"
#include "GraphicsManager.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <functional>


GraphicsManager::~GraphicsManager()
{
    clear();
}

void GraphicsManager::loadResources()
{
    loadShader("src/Shaders/vertex.vert", "src/Shaders/PBR.frag", "PBR");

    // Load fonts, models, or other resources here
    loadModel(engine_util::buildPath("assets/scene.gltf"), "scene");
}

void GraphicsManager::unloadResources()
{
    clear();
}

void GraphicsManager::clear()
{
    shader_map_.clear();
    texture_map_.clear();
    material_map_.clear();
    mesh_map_.clear();
    model_map_.clear();
    font_map_.clear();
}

std::shared_ptr<Shader> GraphicsManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name)
{
    // Check if shader already loaded
    auto it = shader_map_.find(name);
    if (it != shader_map_.end())
    {
        return it->second;
    }

    // Load and compile shader
    auto shader = Shader::createShader(vertexPath, fragmentPath);
    if (shader)
    {
        shader_map_.emplace(name, shader);
    }
    else
    {
        std::cerr << "Failed to load shader: " << name << std::endl;
    }
    return shader;
}

std::shared_ptr<Shader> GraphicsManager::getShader(const std::string& name)
{
    auto it = shader_map_.find(name);
    if (it != shader_map_.end())
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

std::shared_ptr<Texture> GraphicsManager::loadTexture(const std::string& path, aiTextureType type, const aiScene* scene)
{
    // Use canonical path as key to avoid duplicates
    std::string full_path = path;
    if (std::filesystem::exists(path))
    {
        full_path = std::filesystem::canonical(path).string();
    }

    auto it = texture_map_.find(full_path);
    if (it != texture_map_.end())
    {
        return it->second;
    }

    // Check if texture is embedded
    if (path.substr(0, 1) == "*") // Embedded texture
    {
        unsigned int textureIndex = std::stoi(path.substr(1));
        aiTexture* aiTex = scene->mTextures[textureIndex];

        // Load embedded texture
        auto texture = Texture::createEmbeddedTexture(aiTex, type);
        if (texture)
        {
            texture_map_.emplace(full_path, texture);
        }
        else
        {
            std::cerr << "Failed to load embedded texture at index: " << textureIndex << std::endl;
        }
        return texture;
    }
    else
    {
        // Load texture from file
        auto texture = Texture::createTexture(full_path, type);
        if (texture)
        {
            texture_map_.emplace(full_path, texture);
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
    auto it = texture_map_.find(path);
    if (it != texture_map_.end())
    {
        return it->second;
    }
    else
    {
        std::cerr << "Texture not found: " << path << std::endl;
        return nullptr;
    }
}

// Material management

std::shared_ptr<Material> GraphicsManager::createMaterial(const std::string& name,
    const glm::vec4& albedo,
    float metallic,
    float roughness,
    float AO,
    const std::shared_ptr<Texture>& albedoTexture,
    const std::shared_ptr<Texture>& normalTexture,
    const std::shared_ptr<Texture>& metallicTexture,
    const std::shared_ptr<Texture>& roughnessTexture,
    const std::shared_ptr<Texture>& AOTexture,
    const std::shared_ptr<Texture>& emissiveTexture)
{
    // Check if material already exists
    auto it = material_map_.find(name);
    if (it != material_map_.end())
    {
        return it->second;
    }

    // Create new material
    auto material = Material::createMaterial(albedo, metallic, roughness, AO,
        albedoTexture, normalTexture, metallicTexture,
        roughnessTexture, AOTexture, emissiveTexture);
    material_map_.emplace(name, material);

    return material;
}

std::shared_ptr<Material> GraphicsManager::getMaterial(const std::string& name)
{
    auto it = material_map_.find(name);
    if (it != material_map_.end())
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
    auto it = mesh_map_.find(name);
    if (it != mesh_map_.end())
    {
        return it->second;
    }

    // Create new mesh
    auto mesh = Mesh::createMesh(vertices, indices, material);
    mesh_map_.emplace(name, mesh);

    return mesh;
}

std::shared_ptr<Mesh> GraphicsManager::getMesh(const std::string& name)
{
    auto it = mesh_map_.find(name);
    if (it != mesh_map_.end())
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

std::shared_ptr<Model> GraphicsManager::createModel(const std::vector<std::shared_ptr<Mesh>>& meshes, const std::string& name)
{
    // Check if model already exists
    auto it = model_map_.find(name);
    if (it != model_map_.end())
    {
        return it->second;
    }

    // Create new model
    auto model = Model::createModel(meshes);
    model_map_.emplace(name, model);

    return model;
}

std::shared_ptr<Model> GraphicsManager::getModel(const std::string& name)
{
    auto it = model_map_.find(name);
    if (it != model_map_.end())
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
    auto it = model_map_.find(name);
    if (it != model_map_.end())
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
        aiProcess_EmbedTextures);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::string directory = std::filesystem::path(path).parent_path().string();

    // Process all meshes
    std::vector<std::shared_ptr<Mesh>> meshes;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* ai_mesh = scene->mMeshes[i];
        auto mesh = processMesh(ai_mesh, scene, directory);
        if (mesh)
        {
            meshes.emplace_back(mesh);
        }
    }

    // Create the model
    auto model = createModel(meshes, name);

    return model;
}



std::shared_ptr<Mesh> GraphicsManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
{
    // Generate unique mesh name
    std::string mesh_name = mesh->mName.C_Str();
    if (mesh_name.empty())
    {
        mesh_name = "mesh_" + std::to_string(mesh_map_.size());
    }

    // Check if mesh already exists
    auto it = mesh_map_.find(mesh_name);
    if (it != mesh_map_.end())
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
        material_name = "material_" + std::to_string(material_map_.size());
    }

    // Check if material already exists
    auto it = material_map_.find(material_name);
    if (it != material_map_.end())
    {
        return it->second;
    }

    // Material properties
    glm::vec4 albedo(1.0f);
    float metallic = 0.0f;
    float roughness = 1.0f;
    float AO = 1.0f;

    // Load textures
    std::shared_ptr<Texture> albedoTexture;
    std::shared_ptr<Texture> normalTexture;
    std::shared_ptr<Texture> metallicTexture;
    std::shared_ptr<Texture> roughnessTexture;
    std::shared_ptr<Texture> AOTexture;
    std::shared_ptr<Texture> emissiveTexture;

    // Load the textures based on aiTextureType
    auto albedoTextures = loadMaterialTextures(mat, aiTextureType_BASE_COLOR, directory, scene);
    if (!albedoTextures.empty()) albedoTexture = albedoTextures[0];

    auto normalTextures = loadMaterialTextures(mat, aiTextureType_NORMAL_CAMERA, directory, scene);
    if (!normalTextures.empty()) normalTexture = normalTextures[0];

    auto metallicTextures = loadMaterialTextures(mat, aiTextureType_METALNESS, directory, scene);
    if (!metallicTextures.empty()) metallicTexture = metallicTextures[0];

    auto roughnessTextures = loadMaterialTextures(mat, aiTextureType_DIFFUSE_ROUGHNESS, directory, scene);
    if (!roughnessTextures.empty()) roughnessTexture = roughnessTextures[0];

    auto AOTextures = loadMaterialTextures(mat, aiTextureType_AMBIENT_OCCLUSION, directory, scene);
    if (!AOTextures.empty()) AOTexture = AOTextures[0];

    auto emissiveTextures = loadMaterialTextures(mat, aiTextureType_EMISSION_COLOR, directory, scene);
    if (!emissiveTextures.empty()) emissiveTexture = emissiveTextures[0];

    // Create the material
    auto material = createMaterial(material_name, albedo, metallic, roughness, AO,
        albedoTexture, normalTexture, metallicTexture,
        roughnessTexture, AOTexture, emissiveTexture);

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


// Font management

std::shared_ptr<Font> GraphicsManager::loadFont(const std::string& fontPath, float fontSize)
{
    // Check if font already loaded
    auto it = font_map_.find(fontPath);
    if (it != font_map_.end())
    {
        return it->second;
    }

    // Load font
    auto font = std::make_shared<Font>(fontPath, fontSize);
    font_map_.emplace(fontPath, font);
    return font;
}

std::shared_ptr<Font> GraphicsManager::getFont(const std::string& name)
{
    auto it = font_map_.find(name);
    if (it != font_map_.end())
    {
        return it->second;
    }
    else
    {
        std::cerr << "Font not found: " << name << std::endl;
        return nullptr;
    }
}
