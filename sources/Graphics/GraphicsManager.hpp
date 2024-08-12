#pragma once
#include "Graphics/ShaderProgram.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Color.hpp"
struct vertex;
class mesh;
class model;
class material;
class texture;

class graphics_manager
{
public:
    // resource storage
    static std::unordered_map<std::string, std::unique_ptr<shader_program>> shader_map;
    static std::unordered_map<std::string, std::unique_ptr<texture>> texture_map;
    static std::unordered_map<std::string, std::unique_ptr<mesh>> mesh_map;
    static std::unordered_map<std::string, std::unique_ptr<model>> model_map;
    static std::unordered_map<std::string, std::unique_ptr<material>> material_map;
    // loads (and generates) a shader program from file loading vertex and fragment shader's source code.
    static shader_program& load_shader(const char* vShaderFile, const char* fShaderFile, const std::string& name);
    // retrieves a stored shader
    static shader_program& get_shader(const std::string& name);
    // loads (and generates) a texture from file
    static texture& load_texture(const char* file, bool alpha, const std::string& name);
    // retrieves a stored texture
    static texture& get_texture(const std::string& name);
    static mesh& create_mesh(std::string name, std::vector<vertex> vertices, std::string material_name);
    static mesh& get_mesh(const std::string& name);
    // properly de-allocates all loaded resources
    static model& create_model(const std::vector<std::string>& mesh_names, const std::string& name);
    static model& get_model(const std::string& name);
    static material& create_material(std::string& name, std::string& diffuse_path, std::string& specular_path,
                                     float& shininess, glm::vec3& ambient_color, glm::vec3& diffuse_color,
                                     glm::vec3& specular_color);
    static material& get_material(const std::string& name);
    static void Clear();

    static std::vector<std::string> load_obj(const char* file);
    static std::vector<std::string> load_mtl(const char* file);

private:
    graphics_manager()
    {
    }
};
