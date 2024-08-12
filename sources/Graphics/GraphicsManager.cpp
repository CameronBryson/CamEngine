#include "GraphicsManager.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Material.hpp"
std::unordered_map<std::string, std::unique_ptr<shader_program>> graphics_manager::shader_map;
std::unordered_map<std::string, std::unique_ptr<texture>> graphics_manager::texture_map;
std::unordered_map<std::string, std::unique_ptr<mesh>> graphics_manager::mesh_map;
std::unordered_map<std::string, std::unique_ptr<model>> graphics_manager::model_map;
std::unordered_map<std::string, std::unique_ptr<material>> graphics_manager::material_map;

shader_program & graphics_manager::load_shader(const char * vShaderFile, const char * fShaderFile, const std::string & name)
{
    shader_map[name] = std::make_unique<shader_program>(vShaderFile, fShaderFile);
    return *shader_map[name];
}

shader_program & graphics_manager::get_shader(const std::string & name)
{
    return *shader_map[name];
}

texture & graphics_manager::load_texture(const char * file, bool alpha, const std::string & name)
{
    texture_map[name] = std::make_unique<texture>(file, alpha);
    return *texture_map[name];
}

texture & graphics_manager::get_texture(const std::string & name)
{
    return *texture_map[name];
}

mesh & graphics_manager::create_mesh(std::string name,  std::vector<vertex> vertices, std::string material_name)
{
    mesh_map[name] = std::make_unique<mesh>(vertices,material_name);
    return *mesh_map[name];
}

mesh & graphics_manager::get_mesh(const std::string & name)
{
    return *mesh_map[name];
}

model & graphics_manager::create_model(const std::vector<std::string> & mesh_names, const std::string & name)
{
    model_map[name] = std::make_unique<model>(mesh_names);
    return *model_map[name];
}

model & graphics_manager::get_model(const std::string & name)
{
    return *model_map[name];
}

void graphics_manager::Clear()
{
    // (properly) delete all shaders
    for( auto & iter : shader_map )
        glDeleteProgram(iter.second->ID);
    // (properly) delete all textures
    for( auto & iter : texture_map )
    {
    }
    //glDeleteTextures(1, &iter.second->ID);
}

std::vector<std::string> graphics_manager::load_obj(const char * file)
{
    std::ifstream obj_file(file);
    if( ! obj_file.is_open() )
    {
        std::cerr << "Failed to open file: " << file << std::endl;
        return {};
    }

    std::vector<std::string> mesh_names;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<vertex> vertices;
    std::string currentMaterial = "Default";

    std::string line, currentMeshName;
    bool firstObject = true;

    auto process_mesh = [&]()
    {
        for( size_t i = 0; i < vertexIndices.size(); i++ )
        {
            vertex v{};
            v.position = temp_vertices[vertexIndices[i]];
            v.texture_coordinates = temp_uvs[uvIndices[i]];
            v.normal = temp_normals[normalIndices[i]];
            vertices.push_back(v);
        }
        //create_mesh(currentMeshName, vertices, "Default");

        create_mesh(currentMeshName, vertices, currentMaterial);
        mesh_names.push_back(currentMeshName);
    };

    while( std::getline(obj_file, line) )
    {
        std::istringstream line_stream(line);
        std::string prefix;
        line_stream >> prefix;

        if( prefix == "o" )
        {
            if( ! firstObject ) process_mesh();
            firstObject = false;
            line_stream >> currentMeshName;
        }
        else if( prefix == "v" )
        {
            glm::vec3 vertex;
            line_stream >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if( prefix == "vt" )
        {
            glm::vec2 uv;
            line_stream >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if( prefix == "vn" )
        {
            glm::vec3 normal;
            line_stream >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if( prefix == "f" )
        {
            for( int i = 0; i < 3; i++ )
            {
                unsigned int vertexIndex, uvIndex, normalIndex;
                char slash;
                line_stream >> vertexIndex >> slash >> uvIndex >> slash >> normalIndex;
                vertexIndices.push_back(vertexIndex - 1);
                uvIndices.push_back(uvIndex - 1);
                normalIndices.push_back(normalIndex - 1);
            }
        }
        else if( prefix == "usemtl" )
        {
            line_stream >> currentMaterial;
        }
    }

    if( ! currentMeshName.empty() ) process_mesh();

    return mesh_names;
}

material & graphics_manager::create_material(std::string & name, std::string & diffuse_path, std::string & specular_path, float & shininess,
    glm::vec3 & ambient_color, glm::vec3 & diffuse_color, glm::vec3 & specular_color)
{
    material_map[name] = std::make_unique<material>(diffuse_path, specular_path, shininess, ambient_color, diffuse_color, specular_color);
    return *material_map[name];
}

material & graphics_manager::get_material(const std::string & name)
{
    return *material_map[name];
}

std::vector<std::string> graphics_manager::load_mtl(const char * file)
{
    std::ifstream mtl_file(file);
    if( ! mtl_file.is_open() )
    {
        std::cerr << "Failed to open file: " << file << std::endl;
        return {};
    }

    std::vector<std::string> material_names;
    std::string line;
    std::string currentMaterialName;
    std::string diffuse_path, specular_path;
    float shininess = 0.0f;
    glm::vec3 ambient_color(0.0f), diffuse_color(0.0f), specular_color(0.0f);

    while( std::getline(mtl_file, line) )
    {
        std::istringstream line_stream(line);
        std::string prefix;
        line_stream >> prefix;

        if( prefix == "newmtl" )
        {
            if( ! currentMaterialName.empty() )
            {
                create_material(currentMaterialName, diffuse_path, specular_path, shininess, ambient_color, diffuse_color, specular_color);
                material_names.push_back(currentMaterialName);
            }
            line_stream >> currentMaterialName;
            diffuse_path.clear();
            specular_path.clear();
            shininess = 0.0f;
            ambient_color = glm::vec3(0.0f);
            diffuse_color = glm::vec3(0.0f);
            specular_color = glm::vec3(0.0f);
        }
        else if( prefix == "map_Kd" )
        {
            line_stream >> diffuse_path;
        }
        else if( prefix == "map_Ks" )
        {
            line_stream >> specular_path;
        }
        else if( prefix == "Ns" )
        {
            line_stream >> shininess;
        }
        else if( prefix == "Ka" )
        {
            line_stream >> ambient_color.r >> ambient_color.g >> ambient_color.b;
        }
        else if( prefix == "Kd" )
        {
            line_stream >> diffuse_color.r >> diffuse_color.g >> diffuse_color.b;
        }
        else if( prefix == "Ks" )
        {
            line_stream >> specular_color.r >> specular_color.g >> specular_color.b;
        }
    }

    if( ! currentMaterialName.empty() )
    {
        create_material(currentMaterialName, diffuse_path, specular_path, shininess, ambient_color, diffuse_color, specular_color);
        material_names.push_back(currentMaterialName);
    }

    return material_names;
}