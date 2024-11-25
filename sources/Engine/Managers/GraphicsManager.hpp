#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "glm/vec3.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Model.hpp"
#include "Engine/Graphics/Material.hpp"
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Graphics/Mesh.hpp"

class GraphicsManager
{
public:
    GraphicsManager() = default;

    // Load and generate a shader program from vertex and fragment shader source files
    std::shared_ptr<Shader> loadShader(const std::string& vShaderFile, const std::string& fShaderFile, const std::string& name);
    // Retrieve a stored shader
    std::shared_ptr<Shader> getShader(const std::string& name);

    // Load and generate a texture from a file
    std::shared_ptr<Texture> loadTexture(const std::string& file, const std::string& name);
    // Retrieve a stored texture
    std::shared_ptr<Texture> getTexture(const std::string& name);

    // Create and store a mesh
    std::shared_ptr<Mesh> createMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::string& material_name);
    // Retrieve a stored mesh
    std::shared_ptr<Mesh> getMesh(const std::string& name);

    // Create and store a material
    std::shared_ptr<Material> createMaterial(const std::string& name, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum,
        const std::string& map_Ka_path, const std::string& map_Kd_path, const std::string& map_Ks_path, const std::string& map_Ns_path,
        const std::string& map_d_path, const std::string& map_bump_path);
    // Retrieve a stored material
    std::shared_ptr<Material> getMaterial(const std::string& name);

    // Create and store a model
    std::shared_ptr<Model> createModel(const std::vector<std::string>& mesh_names, const std::string& name);
    std::shared_ptr<Model> createModelFromObj(const std::string& file, const std::string& name);
    // Retrieve a stored model
    std::shared_ptr<Model> getModel(const std::string& name);

    void Clear();

    std::vector<std::string> loadObj(const std::string& file);
    std::vector<std::string> loadMtl(const std::string& file);

private:
    // Maps to store shared pointers to resources
    std::unordered_map<std::string, std::shared_ptr<Shader>> shader_map;
    std::unordered_map<std::string, std::shared_ptr<Texture>> texture_map;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_map;
    std::unordered_map<std::string, std::shared_ptr<Model>> model_map;
    std::unordered_map<std::string, std::shared_ptr<Material>> material_map;
};
