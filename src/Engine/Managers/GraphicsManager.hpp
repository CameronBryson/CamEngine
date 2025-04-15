#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <string_view>

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
#include <string_view>
class GraphicsManager
{
public:
    GraphicsManager() = default;
    ~GraphicsManager();

    void loadResources();
    void unloadResources();

    void reloadShaders();

    // Shader management
    std::shared_ptr<Shader> loadShader(std::string_view vertexPath, std::string_view fragmentPath, std::string name);
	std::shared_ptr<Shader> loadShader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath, std::string name);
    std::shared_ptr<Shader> getShader(const std::string& name);

    // Texture management
    std::shared_ptr<Texture> loadTexture(const std::string& path, aiTextureType type, const aiScene* scene = nullptr);
    std::shared_ptr<Texture> getTexture(const std::string& path);

    // Material management
    std::shared_ptr<Material> getMaterial(const std::string& name);

    // Model and Scene management
    std::shared_ptr<Model> getModel(const std::string& name);
    std::shared_ptr<Model> loadModel(std::string_view path, std::string name);



    std::shared_ptr<EnvironmentMap> loadEnvironmentMap(std::string name, std::string_view hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader);
    std::shared_ptr<EnvironmentMap> getEnvironmentMap(const std::string& name);


    // Resource cleanup
    void clear();

private:
    // Resource maps
    std::unordered_map<std::string, std::shared_ptr<Shader>> mShaderMap;
    std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureMap;
    std::unordered_map<std::string, std::shared_ptr<Material>> mMaterialMap;
    std::unordered_map<std::string, std::shared_ptr<Model>> mModelMap;
    std::unordered_map<std::string, std::shared_ptr<EnvironmentMap>> mEnvironmentMap;


    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
    std::shared_ptr<Material> loadMaterial(aiMaterial* mat, const std::string& directory, const aiScene* scene);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& directory, const aiScene* scene);
    void processNode(aiNode* node, const aiScene* scene, const std::string& directory, const glm::mat4& parentTransform, std::vector<MeshInstance>& meshInstances);
    glm::mat4 aiMatrixToGlm(const aiMatrix4x4& aiMat);
};
