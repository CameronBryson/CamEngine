#include "pch.hpp"  
#include "Shader.hpp"  
#include <sstream>  
#include <fstream>  
#include <iostream>  
#include "Engine/Util/EngineUtil.hpp"  
#include "Engine/Util/OpenGLUtil.hpp"  
#include "glm/mat4x4.hpp"  
#include "glm/vec2.hpp"  
#include "glm/vec3.hpp"  
#include "glm/vec4.hpp"  

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)  
{  
    // 1. retrieve the vertex/fragment source code from filePath  
    std::string vertexCode;  
    std::string fragmentCode;  
    std::ifstream vShaderFile;  
    std::ifstream fShaderFile;  
    std::cout << "vertexPath: " << vertexPath << '\n';  
    std::cout << "fragmentPath: " << fragmentPath << '\n';  
    // ensure ifstream objects can throw exceptions:  
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);  
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);  

    try  
    {  
        // open files  
        vShaderFile.open(vertexPath);  
        fShaderFile.open(fragmentPath);  

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
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << '\n';  
    }  
    const char* vShaderCode = vertexCode.c_str();  
    const char* fShaderCode = fragmentCode.c_str();  
    // vertex shader  
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);  
    GL_CHECK(glShaderSource(vertex, 1, &vShaderCode, NULL));  
    GL_CHECK(glCompileShader(vertex));  
    checkCompileError(vertex, "VERTEX");  
    // fragment Shader  
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);  
    GL_CHECK(glShaderSource(fragment, 1, &fShaderCode, NULL));  
    GL_CHECK(glCompileShader(fragment));  
    checkCompileError(fragment, "FRAGMENT");  
    // shader Program  
    unsigned int ID = glCreateProgram();  
    GL_CHECK(glAttachShader(ID, vertex));  
    GL_CHECK(glAttachShader(ID, fragment));  
    GL_CHECK(glLinkProgram(ID));  
    checkCompileError(ID, "PROGRAM");  
    // delete the shaders as they're linked into our program now and no longer necessary  
    GL_CHECK(glDeleteShader(vertex));  
    GL_CHECK(glDeleteShader(fragment));  

    shaderID = ID;  
}  

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)  
{  
    // 1. retrieve the vertex/fragment/geometry source code from filePath  
    std::string vertexCode, fragmentCode, geometryCode;  
    std::ifstream vShaderFile, fShaderFile, gShaderFile;  

    std::cout << "vertexPath: " << vertexPath << '\n';  
    std::cout << "fragmentPath: " << fragmentPath << '\n';  
    std::cout << "geometryPath: " << geometryPath << '\n';  

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);  
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);  
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);  

    try  
    {  
        // open files  
        vShaderFile.open(vertexPath);  
        fShaderFile.open(fragmentPath);  
        gShaderFile.open(geometryPath);  

        std::stringstream vShaderStream, fShaderStream, gShaderStream;  
        vShaderStream << vShaderFile.rdbuf();  
        fShaderStream << fShaderFile.rdbuf();  
        gShaderStream << gShaderFile.rdbuf();  

        vShaderFile.close();  
        fShaderFile.close();  
        gShaderFile.close();  

        vertexCode = vShaderStream.str();  
        fragmentCode = fShaderStream.str();  
        geometryCode = gShaderStream.str();  
    }  
    catch (std::ifstream::failure& e)  
    {  
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << '\n';  
    }  

    // 2. compile shaders  
    const char* vShaderCode = vertexCode.c_str();  
    const char* fShaderCode = fragmentCode.c_str();  
    const char* gShaderCode = geometryCode.c_str();  

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);  
    GL_CHECK(glShaderSource(vertex, 1, &vShaderCode, NULL));  
    GL_CHECK(glCompileShader(vertex));  
    checkCompileError(vertex, "VERTEX");  

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);  
    GL_CHECK(glShaderSource(fragment, 1, &fShaderCode, NULL));  
    GL_CHECK(glCompileShader(fragment));  
    checkCompileError(fragment, "FRAGMENT");  

    unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);  
    GL_CHECK(glShaderSource(geometry, 1, &gShaderCode, NULL));  
    GL_CHECK(glCompileShader(geometry));  
    checkCompileError(geometry, "GEOMETRY");  

    // 3. link the program  
    unsigned int ID = glCreateProgram();  
    GL_CHECK(glAttachShader(ID, vertex));  
    GL_CHECK(glAttachShader(ID, fragment));  
    GL_CHECK(glAttachShader(ID, geometry));  
    GL_CHECK(glLinkProgram(ID));  
    checkCompileError(ID, "PROGRAM");  

    // 4. delete shaders, as they're linked into our program now and no longer necessary  
    GL_CHECK(glDeleteShader(vertex));  
    GL_CHECK(glDeleteShader(fragment));  
    GL_CHECK(glDeleteShader(geometry));  

    shaderID = ID;  
}

Shader::Shader(const std::string& computePath)
{
    std::string computeCode;
    std::ifstream cShaderFile;

    std::cout << "computePath: " << computePath << '\n';

    // ensure ifstream objects can throw exceptions:
    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open file
        cShaderFile.open(computePath);

        std::stringstream cShaderStream;
        // read file's buffer contents into streams
        cShaderStream << cShaderFile.rdbuf();
        // close file handlers
        cShaderFile.close();
        // convert stream into string
        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << '\n';
    }

    const char* cShaderCode = computeCode.c_str();

    // compute shader
    unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
    GL_CHECK(glShaderSource(compute, 1, &cShaderCode, NULL));
    GL_CHECK(glCompileShader(compute));
    checkCompileError(compute, "COMPUTE");

    // shader Program
    unsigned int ID = glCreateProgram();
    GL_CHECK(glAttachShader(ID, compute));
    GL_CHECK(glLinkProgram(ID));
    checkCompileError(ID, "PROGRAM");

    // delete the shader as it's linked into our program now and no longer necessary
    GL_CHECK(glDeleteShader(compute));

    shaderID = ID;
    isComputeShader = true;
}


Shader::~Shader() 
{
    deleteShader();
}  

void Shader::use() const { GL_CHECK(glUseProgram(shaderID)); }

void Shader::dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const
{
    if (!isComputeShader) {
        std::cout << "ERROR::SHADER::NOT_A_COMPUTE_SHADER\n";
        return;
    }
    use();
    GL_CHECK(glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ));

}

void Shader::setBool(const std::string& name, bool value) const  
{  
    GL_CHECK(glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value));  
}  

void Shader::setInt(const std::string& name, int value) const  
{  
    GL_CHECK(glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value));  
}  

void Shader::setFloat(const std::string& name, float value) const  
{  
    GL_CHECK(glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value));  
}  

void Shader::setVec2(const std::string& name, const glm::vec2& value) const  
{  
    GL_CHECK(glUniform2fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]));  
}  

void Shader::setVec3(const std::string& name, const glm::vec3& value) const  
{  
    GL_CHECK(glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]));  
}  

void Shader::setVec4(const std::string& name, const glm::vec4& value) const  
{  
    GL_CHECK(glUniform4fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]));  
}  

void Shader::setMat4(const std::string& name, const glm::mat4& value) const  
{  
    GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]));  
}  

void Shader::deleteShader() {
    //GL_CHECK(glDeleteProgram(shaderID));
    glDeleteProgram(shaderID);
}  

void Shader::checkCompileError(unsigned shader, const std::string& type)  
{  
    GLint success;  
    GLchar infoLog[1024];  
    if (type != "PROGRAM")  
    {  
        GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));  
        if (!success)  
        {  
            GL_CHECK(glGetShaderInfoLog(shader, 1024, NULL, infoLog));  
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"  
                << infoLog << "\n -- --------------------------------------------------- -- " << '\n';  
        }  
    }  
    else  
    {  
        GL_CHECK(glGetProgramiv(shader, GL_LINK_STATUS, &success));  
        if (!success)  
        {  
            GL_CHECK(glGetProgramInfoLog(shader, 1024, NULL, infoLog));  
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"  
                << infoLog << "\n -- --------------------------------------------------- -- " << '\n';  
        }  
    }  
}
