#pragma once
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texure.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>
struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
};
class mesh;
class graphics_manager
{
public:
    // resource storage
    static std::unordered_map<std::string, std::unique_ptr<shader_program>> shader_map;
    static std::unordered_map<std::string, std::unique_ptr<texture>> texture_map;
    static std::unordered_map<std::string, std::unique_ptr<mesh>> mesh_map;
    // loads (and generates) a shader program from file loading vertex and fragment shader's source code.
    static shader_program& load_shader (const char *vShaderFile, const char *fShaderFile, std::string name);
    // retrieves a stored shader
    static shader_program& get_shader(std::string name);
    // loads (and generates) a texture from file
    static texture& load_texture(const char *file, bool alpha, std::string name);
    // retrieves a stored texture
    static texture& get_texture(std::string name);
    static mesh& load_mesh(const char* file, std::string name);
    static mesh& get_mesh(std::string name);
    // properly de-allocates all loaded resources
    static void Clear();

    static void load_obj(const char *file, std::vector<vertex> &verticies);


private:
    graphics_manager() { }
};

class mesh
{
public:
    mesh(const char* obj_file)
    {
        graphics_manager::load_obj(obj_file,vertices);
        index_count = vertices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(),vertices.data(), GL_STATIC_DRAW);


        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texture_coordinates));


        glBindVertexArray(0);
    }
    void draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,index_count);
        //glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
    }
private:
    unsigned int VAO = 0, VBO = 0, index_count;
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
};
