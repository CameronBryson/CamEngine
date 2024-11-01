#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
shader_program::shader_program(const char* vertexPath, const char* fragmentPath)
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
	//vShaderFile.open(vertexPath);
	auto path = engine_util::build_path(vertexPath);

	vShaderFile.open(engine_util::build_path(vertexPath));

	//fShaderFile.open(fragmentPath);
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
    catch( std::ifstream::failure& e )
    {
	std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    ID = opengl_util::create_shader(vertexCode, fragmentCode);
}
void shader_program::use() const
{
    opengl_util::use_shader(ID);
}
void shader_program::setBool(const std::string& name, bool value) const
{
    opengl_util::set_shader_bool(ID, name, value);
}
void shader_program::setInt(const std::string& name, int value) const
{
    opengl_util::set_shader_int(ID, name, value);
}
void shader_program::setFloat(const std::string& name, float value) const
{
    opengl_util::set_shader_float(ID, name, value);
}
void shader_program::setVec2(const std::string& name, const glm::vec2& value) const
{
    opengl_util::set_shader_vec2(ID, name, value);
}
void shader_program::setVec3(const std::string& name, const glm::vec3& value) const
{
    opengl_util::set_shader_vec3(ID, name, value);
}
void shader_program::setVec4(const std::string& name, const glm::vec4& value) const
{
    opengl_util::set_shader_vec4(ID, name, value);
}
void shader_program::setMat4(const std::string& name, const glm::mat4& mat) const
{
    opengl_util::set_shader_mat4(ID, name, mat);
}
