#pragma once
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texure.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class graphics_object;
class graphics_manager
{
public:
    // resource storage
    static std::unordered_map<std::string, std::unique_ptr<shader_program>> shader_map;
    static std::unordered_map<std::string, std::unique_ptr<texture>> texture_map;
    static std::unordered_map<std::string, std::unique_ptr<graphics_object>> object_map;
    // loads (and generates) a shader program from file loading vertex and fragment shader's source code.
    static shader_program& load_shader (const char *vShaderFile, const char *fShaderFile, std::string name);
    // retrieves a stored shader
    static shader_program& get_shader(const std::string name);
    // loads (and generates) a texture from file
    static texture& load_texture(const char *file, bool alpha, const std::string name);
    // retrieves a stored texture
    static texture& get_texture(const std::string name);
    static graphics_object& load_obj(const char* file, std::string name);
    static graphics_object& get_obj(std::string name);
    // properly de-allocates all loaded resources
    static void Clear();

    static void load_obj(const char *file, std::vector<float> &verticies, std::vector<unsigned int> &indicies);

private:
    graphics_manager() { }
};

class graphics_object
{
public:
    graphics_object(const char* obj_file)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        graphics_manager::load_obj(obj_file,vertices,indices);
        index_count = indices.size();
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);


        glBindVertexArray(0);
    }
    void enable()
    {
        glBindVertexArray(VAO);
    }
    void disable()
    {
        glBindVertexArray(0);
    }
    unsigned int get_count()
    {
        return index_count;
    }
private:
    unsigned int VAO = 0, VBO = 0, EBO = 0, index_count;
};
