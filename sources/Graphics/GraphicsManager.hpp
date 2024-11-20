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
    ShaderProgram& loadShader(const std::string& vShaderFile, const std::string& fShaderFile, const std::string& name);
    // retrieves a stored shader
    ShaderProgram& getShader(const std::string& name);
    // loads (and generates) a texture from file
    Texture& loadTexture(const std::string& file, const std::string& name);
    // retrieves a stored texture
    Texture& getTexture(const std::string& name);
    Mesh& createMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::string& material_name);
    Mesh& getMesh(const std::string& name);
    // properly de-allocates all loaded resources
    Material& createMaterial(const std::string& name, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum, const std::string& map_Ka_path,
	const std::string& map_Kd_path, const std::string& map_Ks_path, const std::string& map_Ns_path, const std::string& map_d_path,
	const std::string& map_bump_path);
    Material& getMaterial(const std::string& name);
    Model& createModel(const std::vector<std::string>& mesh_names, const std::string& name);
    Model& createModelFromObj(const std::string& file, const std::string& name);
    Model& getModel(const std::string& name);
    void Clear();

    std::vector<std::string> loadObj(const std::string& file);
    std::vector<std::string> loadMtl(const std::string& file);

private:
    std::unordered_map<std::string, ShaderProgram> shader_map;
    std::unordered_map<std::string, Texture> texture_map;
    std::unordered_map<std::string, Mesh> mesh_map;
    std::unordered_map<std::string, Model> model_map;
    std::unordered_map<std::string, Material> material_map;
};
