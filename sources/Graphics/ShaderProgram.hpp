#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include "platform.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Engine/EngineUtil.hpp>

#include "OpenGLUtil.hpp"

class shader_program
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    shader_program(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(engine_util::build_path(vertexPath));
            fShaderFile.open(engine_util::build_path(fragmentPath));
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        ID = opengl_util::create_shader(vertexCode,fragmentCode);

    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    {
        opengl_util::use_shader(ID);
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        opengl_util::set_shader_bool(ID,name,value);
    }

    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        opengl_util::set_shader_int(ID,name,value);
    }

    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        opengl_util::set_shader_float(ID,name,value);
    }

    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        opengl_util::set_shader_vec2(ID,name,value);
    }


    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        opengl_util::set_shader_vec3(ID,name,value);
    }


    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        opengl_util::set_shader_vec4(ID,name,value);
    }


    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        opengl_util::set_shader_mat4(ID,name,mat);
    }

};
#endif
