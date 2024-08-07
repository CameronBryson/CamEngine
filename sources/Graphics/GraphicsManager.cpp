#include "GraphicsManager.hpp"
std::unordered_map<std::string,std::unique_ptr<shader_program>> graphics_manager::shader_map;
std::unordered_map<std::string, std::unique_ptr<texture>> graphics_manager::texture_map;
std::unordered_map<std::string, std::unique_ptr<graphics_object>> graphics_manager::object_map;
shader_program& graphics_manager::load_shader(const char *vShaderFile, const char *fShaderFile, std::string name)
{
    shader_map[name] = std::make_unique<shader_program>(vShaderFile,fShaderFile);
    return *shader_map[name];
}
shader_program& graphics_manager::get_shader(const std::string name)
{
    return *shader_map[name];
}
texture& graphics_manager::load_texture(const char *file, bool alpha, const std::string name)
{
    texture_map[name] = std::make_unique<texture>();
    return *texture_map[name];
}
texture &graphics_manager::get_texture(const std::string name)
{
    return *texture_map[name];
}
graphics_object &graphics_manager::load_obj(const char *file, std::string name)
{
    object_map[name] = std::make_unique<graphics_object>(file);
    return *object_map[name];
}
graphics_object &graphics_manager::get_obj(std::string name)
{
    return *object_map[name];
}
void graphics_manager::Clear()
{
    // (properly) delete all shaders
    for (auto &iter : shader_map)
        glDeleteProgram(iter.second->ID);
    // (properly) delete all textures
    for (auto &iter : texture_map)
        glDeleteTextures(1, &iter.second->ID);
}
void graphics_manager::load_obj(const char *file, std::vector<float> &vertices, std::vector<unsigned int> &indices)
{
    std::ifstream obj_file(file);
    if (!obj_file.is_open())
    {
        std::cerr << "Failed to open file: " << file << std::endl;
        return;
    }

    std::string line;
    while (std::getline(obj_file, line))
    {
        std::istringstream line_stream(line);
        std::string prefix;
        line_stream >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            line_stream >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if (prefix == "f")
        {
            unsigned int x,y,z;
            line_stream >> x >> y >>z;
            indices.push_back(x-1);
            indices.push_back(y-1);
            indices.push_back(z-1);
        }
    }

    obj_file.close();
}
