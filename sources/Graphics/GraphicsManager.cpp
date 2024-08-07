#include "GraphicsManager.hpp"
std::unordered_map<std::string,std::unique_ptr<shader_program>> graphics_manager::shader_map;
std::unordered_map<std::string, std::unique_ptr<texture>> graphics_manager::texture_map;
shader_program& graphics_manager::load_shader(const char *vShaderFile, const char *fShaderFile, std::string name)
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
    texture_map[name] = std::make_unique<texture>();
    return *texture_map[name];
}
texture& graphics_manager::get_texture(const std::string &name)
{
    return *texture_map[name];
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
