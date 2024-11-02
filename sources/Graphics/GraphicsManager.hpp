#pragma once
#include "Graphics/ShaderProgram.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


struct Vertex;
class Mesh;
class Model;
class Material;
class Texture;

class GraphicsManager
{
public:

    // loads (and generates) a shader program from file loading vertex and fragment shader's source code.
    ShaderProgram& loadShader(const char* vShaderFile, const char* fShaderFile, const std::string& name);
    // retrieves a stored shader
    ShaderProgram& getShader(const std::string& name);
    // loads (and generates) a texture from file
    Texture& loadTexture(const char* file, const std::string& name);
    // retrieves a stored texture
    Texture& getTexture(const std::string& name);
    Mesh& createMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::string& material_name);
    Mesh& getMesh(const std::string& name);
    // properly de-allocates all loaded resources
    Model& createModel(const std::vector<std::string>& mesh_names, const std::string& name);
    Model& createModelFromObj(const char* file, const std::string& name);
    Model& getModel(const std::string& name);
    Material& createMaterial(std::string& name, std::string& diffuse_path, std::string& specular_path,
                                     float& shininess, glm::vec3& ambient_color, glm::vec3& diffuse_color,
                                     glm::vec3& specular_color);
    Material& getMaterial(const std::string& name);
    void Clear();

    std::vector<std::string> loadObj(const char* file);
    std::vector<std::string> loadMtl(const char* file);

private:
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shader_map;
    std::unordered_map<std::string, std::unique_ptr<Texture>> texture_map;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mesh_map;
    std::unordered_map<std::string, std::unique_ptr<Model>> model_map;
    std::unordered_map<std::string, std::unique_ptr<Material>> material_map;
};
