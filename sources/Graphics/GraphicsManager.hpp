#pragma once
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texure.hpp"
#include <memory>
#include <string>
#include <unordered_map>
class graphics_manager
{
public:
    // resource storage
    static std::unordered_map<std::string, std::unique_ptr<shader_program>> shader_map;
    static std::unordered_map<std::string, std::unique_ptr<texture>> texture_map;
    // loads (and generates) a shader program from file loading vertex and fragment shader's source code.
    static shader_program& load_shader (const char *vShaderFile, const char *fShaderFile, std::string name);
    // retrieves a stored shader
    static shader_program& get_shader(const std::string &name);
    // loads (and generates) a texture from file
    static texture& load_texture(const char *file, bool alpha, const std::string &name);
    // retrieves a stored texture
    static texture& get_texture(const std::string &name);
    // properly de-allocates all loaded resources
    static void Clear();

private:
    // private constructor, that is we do not want any actual graphics manager objects. Its members and functions should be publicly available (static).
    graphics_manager() { }
    // loads and generates a shader from file
    // loads and generates a shader from file
};
