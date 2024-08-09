#include "GraphicsManager.hpp"
std::unordered_map<std::string,std::unique_ptr<shader_program>> graphics_manager::shader_map;
std::unordered_map<std::string, std::unique_ptr<texture>> graphics_manager::texture_map;
std::unordered_map<std::string, std::unique_ptr<mesh>> graphics_manager::mesh_map;
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
mesh &graphics_manager::load_mesh(const char *file, std::string name)
{
    mesh_map[name] = std::make_unique<mesh>(file);
    return *mesh_map[name];
}
mesh &graphics_manager::get_mesh(std::string name)
{
    return *mesh_map[name];
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
// void graphics_manager::load_obj(const char *file, std::vector<vertex> &vertices, std::vector<unsigned int> &indices)
// {
//     std::ifstream obj_file(file);
//     if (!obj_file.is_open())
//     {
//         std::cerr << "Failed to open file: " << file << std::endl;
//         return;
//     }
//
//     std::string line;
//     while (std::getline(obj_file, line))
//     {
//         std::istringstream line_stream(line);
//         std::string prefix;
//         line_stream >> prefix;
//
//         if (prefix == "v")
//         {
//             float x, y, z;
//             line_stream >> x >> y >> z;
//             vertex v;
//             v.position.x = x;
//             v.position.y = y;
//             v.position.z = z;
//             vertices.push_back(v);
//         }
//         else if (prefix == "f")
//         {
//             unsigned int x,y,z;
//             line_stream >> x >> y >>z;
//             indices.push_back(x-1);
//             indices.push_back(y-1);
//             indices.push_back(z-1);
//         }
//     }
//
//     obj_file.close();
// }
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
