#include "GraphicsManager.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Material.hpp"
std::unordered_map<std::string,std::unique_ptr<shader_program>> graphics_manager::shader_map;
std::unordered_map<std::string, std::unique_ptr<texture>> graphics_manager::texture_map;
std::unordered_map<std::string, std::unique_ptr<mesh>> graphics_manager::mesh_map;
std::unordered_map<std::string, std::unique_ptr<model>> graphics_manager::model_map;
std::unordered_map<std::string, std::unique_ptr<material>> graphics_manager::material_map;
shader_program& graphics_manager::load_shader(const char *vShaderFile, const char *fShaderFile, const std::string &name)
{
    shader_map[name] = std::make_unique<shader_program>(vShaderFile,fShaderFile);
    return *shader_map[name];
}
shader_program& graphics_manager::get_shader(const std::string &name)
{
    return *shader_map[name];
}
texture& graphics_manager::load_texture(const char *file, bool alpha, const std::string &name)
{
    texture_map[name] = std::make_unique<texture>(file, alpha);
    return *texture_map[name];
}
texture &graphics_manager::get_texture(const std::string &name)
{
    return *texture_map[name];
}
mesh &graphics_manager::load_mesh(const char *file,const std::string& material_name, const std::string &name)
{
    mesh_map[name] = std::make_unique<mesh>(file,material_name);
    return *mesh_map[name];
}
mesh &graphics_manager::get_mesh(const std::string &name)
{
    return *mesh_map[name];
}
model& graphics_manager::create_model(const std::vector<std::string>& mesh_names, const std::string& name){
        model_map[name] = std::make_unique<model>(mesh_names);
        return *model_map[name];
}
model &graphics_manager::get_model(const std::string &name)
{
    return *model_map[name];
}
void graphics_manager::Clear()
{
    // (properly) delete all shaders
    for (auto &iter : shader_map)
        glDeleteProgram(iter.second->ID);
    // (properly) delete all textures
    for (auto &iter : texture_map){}
        //glDeleteTextures(1, &iter.second->ID);
}
void graphics_manager::load_obj(const char* file, std::vector<vertex> &vertices)
{
    std::ifstream obj_file(file);
    if (!obj_file.is_open())
    {
        std::cerr << "Failed to open file: " << file << std::endl;
        return;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    std::string line;
    while (std::getline(obj_file, line))
    {
        std::istringstream line_stream(line);
        std::string prefix;
        line_stream >> prefix;

        if (prefix == "v")
        {
            glm::vec3 vertex;
            line_stream >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt")
        {
            glm::vec2 uv;
            line_stream >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn")
        {
            glm::vec3 normal;
            line_stream >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                unsigned int vertexIndex, uvIndex, normalIndex;
                char slash;
                line_stream >> vertexIndex >> slash >> uvIndex >> slash >> normalIndex;
                vertexIndices.push_back(vertexIndex - 1);
                uvIndices.push_back(uvIndex - 1);
                normalIndices.push_back(normalIndex - 1);
            }
        }
    }

    for (size_t i = 0; i < vertexIndices.size(); i++)
    {
        vertex v{};
        v.position = temp_vertices[vertexIndices[i]];
        v.texture_coordinates = temp_uvs[uvIndices[i]];
        v.normal = temp_normals[normalIndices[i]];
        vertices.push_back(v);
    }
}
material& graphics_manager::load_material(const char* file, const std::string& name)
{
        material_map[name] = std::make_unique<material>(file);
        return *material_map[name];
}
material &graphics_manager::get_material(const std::string &name)
{
    return *material_map[name];
}
void graphics_manager::load_mtl(const char* file, std::string& diffuse_path, std::string& specular_path, float& shininess, glm::vec3& ambient_color, glm::vec3& diffuse_color, glm::vec3& specular_color) {
    std::ifstream mtl_file(file);
    if (!mtl_file.is_open()) {
        std::cerr << "Failed to open file: " << file << std::endl;
        return;
    }

    std::string line;
    while (std::getline(mtl_file, line)) {
        std::istringstream line_stream(line);
        std::string prefix;
        line_stream >> prefix;

        if (prefix == "map_Kd") {
            line_stream >> diffuse_path;
        } else if (prefix == "map_Ks") {
            line_stream >> specular_path;
        } else if (prefix == "Ns") {
            line_stream >> shininess;
        } else if (prefix == "Ka") {
            line_stream >> ambient_color.r >> ambient_color.g >> ambient_color.b;
        } else if (prefix == "Kd") {
            line_stream >> diffuse_color.r >> diffuse_color.g >> diffuse_color.b;
        } else if (prefix == "Ks") {
            line_stream >> specular_color.r >> specular_color.g >> specular_color.b;
        }
    }
}
