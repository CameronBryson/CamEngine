#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "EnvironmentMap.hpp"
#include "Font.hpp"




class GraphicsManager
{
public:
    GraphicsManager() = default;
    ~GraphicsManager();

    void loadResources();
    void unloadResources();

    // Shader management
    std::shared_ptr<Shader> loadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
	std::shared_ptr<Shader> loadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& name);
    std::shared_ptr<Shader> getShader(const std::string& name);

    // Texture management
    std::shared_ptr<Texture> loadTexture(const std::string& path, aiTextureType type, const aiScene* scene = nullptr);
    std::shared_ptr<Texture> getTexture(const std::string& path);

    // Material management
    std::shared_ptr<Material> createMaterial(const std::string& name, const glm::vec4& albedo, float metallic, float roughness, float AO, const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& normalTexture, const std::shared_ptr<Texture>& metallicTexture, const std::shared_ptr<Texture>& roughnessTexture, const std::shared_ptr<Texture>& AOTexture, const std::shared_ptr<Texture>& emissiveTexture, const std::shared_ptr<Texture>& metalRoughTexture);
    std::shared_ptr<Material> getMaterial(const std::string& name);

    // Mesh management
    std::shared_ptr<Mesh> createMesh(const std::string& name,
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::shared_ptr<Material>& material);
    std::shared_ptr<Mesh> getMesh(const std::string& name);

    // Model and Scene management
    std::shared_ptr<Model> getModel(const std::string& name);
    std::shared_ptr<Model> loadModel(const std::string& path, const std::string& name);


    // Font management
    std::shared_ptr<Font> loadFont(const std::string& fontPath, float fontSize);
    std::shared_ptr<Font> getFont(const std::string& name);

    std::shared_ptr<EnvironmentMap> loadEnvironmentMap(const std::string& name, const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader);
    std::shared_ptr<EnvironmentMap> getEnvironmentMap(const std::string& name);


    // Resource cleanup
    void clear();

private:
    // Resource maps
    std::unordered_map<std::string, std::shared_ptr<Shader>> shader_map_;
    std::unordered_map<std::string, std::shared_ptr<Texture>> texture_map_;
    std::unordered_map<std::string, std::shared_ptr<Material>> material_map_;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_map_;
    std::unordered_map<std::string, std::shared_ptr<Model>> model_map_;
    std::unordered_map<std::string, std::shared_ptr<Font>> font_map_;
    std::unordered_map<std::string, std::shared_ptr<EnvironmentMap>> environment_map_;


    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
    std::shared_ptr<Material> loadMaterial(aiMaterial* mat, const std::string& directory, const aiScene* scene);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& directory, const aiScene* scene);
    void processNode(aiNode* node, const aiScene* scene, const std::string& directory, const glm::mat4& parentTransform, std::vector<MeshInstance>& meshInstances);
    glm::mat4 aiMatrixToGlm(const aiMatrix4x4& aiMat);
};
