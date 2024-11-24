#include "ShaderProgram.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

#include "OpenGLUtil.hpp"
#include "Engine/EngineUtil.hpp"

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
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
	auto path = engine_util::buildPath(vertexPath);

	vShaderFile.open(engine_util::buildPath(vertexPath));

	//fShaderFile.open(fragmentPath);
	fShaderFile.open(engine_util::buildPath(fragmentPath));

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
	std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << '\n';
	}

	ID = OpenGlUtil::createShader(vertexCode, fragmentCode);
}
void ShaderProgram::use() const
{
	OpenGlUtil::useShader(ID);
}
void ShaderProgram::setBool(const std::string& name, bool value) const
{
	OpenGlUtil::setShaderBool(ID, name, value);
}
void ShaderProgram::setInt(const std::string& name, int value) const
{
	OpenGlUtil::setShaderInt(ID, name, value);
}
void ShaderProgram::setFloat(const std::string& name, float value) const
{
	OpenGlUtil::setShaderFloat(ID, name, value);
}
void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const
{
	OpenGlUtil::setShaderVec2(ID, name, value);
}
void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const
{
	OpenGlUtil::setShaderVec3(ID, name, value);
}
void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const
{
	OpenGlUtil::setShaderVec4(ID, name, value);
}
void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const
{
	OpenGlUtil::setShaderMat4(ID, name, mat);
}
