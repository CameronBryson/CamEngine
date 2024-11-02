#include "GraphicsManager.hpp"

#include <Engine/EngineUtil.hpp>
#include <fstream>
#include <iostream>

#include "Model.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "OpenGLUtil.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"

ShaderProgram & GraphicsManager::loadShader(const char * vShaderFile, const char * fShaderFile, const std::string & name)
{
    shader_map[name] = std::make_unique<ShaderProgram>(vShaderFile, fShaderFile);
    return *shader_map[name];
}

ShaderProgram & GraphicsManager::getShader(const std::string & name)
{
    return *shader_map[name];
}

Texture & GraphicsManager::loadTexture(const char * file, const std::string & name)
{
    texture_map[name] = std::make_unique<Texture>(file);
    return *texture_map[name];
}

Texture & GraphicsManager::getTexture(const std::string & name)
{
    return *texture_map[name];
}

Mesh & GraphicsManager::createMesh(const std::string& name,  const std::vector<Vertex>& vertices, const std::string& material_name)
{
    mesh_map[name] = std::make_unique<Mesh>(vertices,material_name);
    return *mesh_map[name];
}

Mesh & GraphicsManager::getMesh(const std::string & name)
{
    return *mesh_map[name];
}

Model & GraphicsManager::createModel(const std::vector<std::string> & mesh_names, const std::string & name)
{
    model_map[name] = std::make_unique<Model>(mesh_names);
    return *model_map[name];
}

Model & GraphicsManager::createModelFromObj(const char * file, const std::string & name)
{
    return createModel(loadObj(file), name);
}

Model & GraphicsManager::getModel(const std::string & name)
{
    return *model_map[name];
}

void GraphicsManager::Clear()
{
    // (properly) delete all shaders
    for( auto & iter : shader_map )
        OpenGlUtil::deleteShaderProgram(iter.second->ID);
    // (properly) delete all textures
    for( auto & iter : texture_map )
    {
    }
    //glDeleteTextures(1, &iter.second->ID);
}

std::vector<std::string> GraphicsManager::loadObj(const char * file)
{
    auto test = engine_util::buildPath(file);
    std::ifstream obj_file(engine_util::buildPath(file));
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
    std::vector<Vertex> vertices;
    std::string currentMaterial = "Default";

    std::string line, currentMeshName;
    bool firstObject = true;

    auto process_mesh = [&]()
    {
        for( size_t i = 0; i < vertexIndices.size(); i++ )
        {
            Vertex v{};
            v.position = temp_vertices[vertexIndices[i]];
            v.texture_coordinates = temp_uvs[uvIndices[i]];
            v.normal = temp_normals[normalIndices[i]];
            vertices.push_back(v);
        }
        //create_mesh(currentMeshName, vertices, "Default");

        createMesh(currentMeshName, vertices, currentMaterial);
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
        else if (prefix == "mtllib")
        {
            std::string mtl_file;
            line_stream >> mtl_file;
            loadMtl(mtl_file.c_str());
        }
    }

    if( ! currentMeshName.empty() ) process_mesh();

    return mesh_names;
}

Material & GraphicsManager::createMaterial(std::string & name, std::string & diffuse_path, std::string & specular_path, float & shininess,
    glm::vec3 & ambient_color, glm::vec3 & diffuse_color, glm::vec3 & specular_color)
{
    material_map[name] = std::make_unique<Material>(diffuse_path, specular_path, shininess, ambient_color, diffuse_color, specular_color);
    return *material_map[name];
}

Material & GraphicsManager::getMaterial(const std::string & name)
{
    return *material_map[name];
}

std::vector<std::string> GraphicsManager::loadMtl(const char * file)
{

    std::ifstream mtl_file(engine_util::buildPath(file));
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
                createMaterial(currentMaterialName, diffuse_path, specular_path, shininess, ambient_color, diffuse_color, specular_color);
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
        createMaterial(currentMaterialName, diffuse_path, specular_path, shininess, ambient_color, diffuse_color, specular_color);
        material_names.push_back(currentMaterialName);
    }

    return material_names;
}