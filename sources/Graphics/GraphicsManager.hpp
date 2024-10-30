#pragma once
#include "Graphics/ShaderProgram.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


struct vertex;
class mesh;
class model;
class material;
class texture;

class graphics_manager
{
public:

    // loads (and generates) a shader program from file loading vertex and fragment shader's source code.
    shader_program& load_shader(const char* vShaderFile, const char* fShaderFile, const std::string& name);
    // retrieves a stored shader
    shader_program& get_shader(const std::string& name);
    // loads (and generates) a texture from file
    texture& load_texture(const char* file, const std::string& name);
    // retrieves a stored texture
    texture& get_texture(const std::string& name);
    mesh& create_mesh(const std::string& name, const std::vector<vertex>& vertices, const std::string& material_name);
    mesh& get_mesh(const std::string& name);
    // properly de-allocates all loaded resources
    model& create_model(const std::vector<std::string>& mesh_names, const std::string& name);
    model& create_model_from_obj(const char* file, const std::string& name);
    model& get_model(const std::string& name);
    material& create_material(std::string& name, std::string& diffuse_path, std::string& specular_path,
                                     float& shininess, glm::vec3& ambient_color, glm::vec3& diffuse_color,
                                     glm::vec3& specular_color);
    material& get_material(const std::string& name);
    void Clear();

    std::vector<std::string> load_obj(const char* file);
    std::vector<std::string> load_mtl(const char* file);

private:
    std::unordered_map<std::string, std::unique_ptr<shader_program>> shader_map;
    std::unordered_map<std::string, std::unique_ptr<texture>> texture_map;
    std::unordered_map<std::string, std::unique_ptr<mesh>> mesh_map;
    std::unordered_map<std::string, std::unique_ptr<model>> model_map;
    std::unordered_map<std::string, std::unique_ptr<material>> material_map;
};
