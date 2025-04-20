#include "pch.hpp"
#include "Shader.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "Engine/Util/OpenGLUtil.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <glad/glad.h>
#include "Engine/Util/Logging.hpp"

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath)
{
	LOG_DEBUG(logging::gGraphicsLogger, "Creating shader from VS: {} and FS: {}", vertexPath, fragmentPath);

	std::string vertexCode = loadShaderFile(vertexPath);
	std::string fragmentCode = loadShaderFile(fragmentPath);

	unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode, vertexPath);
	unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentPath);

	// Link the program
	mShaderID = glCreateProgram();
	ASSERT_LOG(logging::gGraphicsLogger, mShaderID != 0, "Failed to create shader program");

	GL_CHECK(glAttachShader(mShaderID, vertex));
	GL_CHECK(glAttachShader(mShaderID, fragment));
	GL_CHECK(glLinkProgram(mShaderID));
	checkCompileError(mShaderID, "PROGRAM", std::string(vertexPath) + " + " + std::string(fragmentPath));

	// Delete the shaders as they're linked into our program now and no longer necessary
	GL_CHECK(glDeleteShader(vertex));
	GL_CHECK(glDeleteShader(fragment));

	LOG_INFO(logging::gGraphicsLogger, "Successfully created shader program (ID: {}) from {} and {}",
			 mShaderID, vertexPath, fragmentPath);
}

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath)
{
	LOG_DEBUG(logging::gGraphicsLogger, "Creating shader from VS: {}, FS: {}, and GS: {}",
			  vertexPath, fragmentPath, geometryPath);

	std::string vertexCode = loadShaderFile(vertexPath);
	std::string fragmentCode = loadShaderFile(fragmentPath);
	std::string geometryCode = loadShaderFile(geometryPath);

	unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode, vertexPath);
	unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentPath);
	unsigned int geometry = compileShader(GL_GEOMETRY_SHADER, geometryCode, geometryPath);

	// Link the program
	mShaderID = glCreateProgram();
	ASSERT_LOG(logging::gGraphicsLogger, mShaderID != 0, "Failed to create shader program");

	GL_CHECK(glAttachShader(mShaderID, vertex));
	GL_CHECK(glAttachShader(mShaderID, fragment));
	GL_CHECK(glAttachShader(mShaderID, geometry));
	GL_CHECK(glLinkProgram(mShaderID));
	checkCompileError(mShaderID, "PROGRAM", std::string(vertexPath) + " + " + std::string(fragmentPath) + " + " + std::string(geometryPath));

	// Delete the shaders as they're linked into our program now and no longer necessary
	GL_CHECK(glDeleteShader(vertex));
	GL_CHECK(glDeleteShader(fragment));
	GL_CHECK(glDeleteShader(geometry));

	LOG_INFO(logging::gGraphicsLogger, "Successfully created shader program (ID: {}) with geometry shader",
			 mShaderID);
}

Shader::~Shader()
{
	deleteShader();
}

Shader::Shader(Shader&& other) noexcept
	: mShaderID(other.mShaderID)
	, mUniformLocationCache(std::move(other.mUniformLocationCache))
{
	// Make sure the other shader doesn't delete our program
	other.mShaderID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other)
	{
		// Clean up our resources
		deleteShader();

		// Move resources from other
		mShaderID = other.mShaderID;
		mUniformLocationCache = std::move(other.mUniformLocationCache);

		// Make sure the other shader doesn't delete our program
		other.mShaderID = 0;
	}
	return *this;
}

void Shader::use() const
{
	GL_CHECK(glUseProgram(mShaderID));
}


void Shader::setBool(const std::string& name, bool value) const
{
	GL_CHECK(glUniform1i(getUniformLocation(name), static_cast<int>(value)));
}

void Shader::setInt(const std::string& name, int value) const
{
	GL_CHECK(glUniform1i(getUniformLocation(name), value));
}

void Shader::setFloat(const std::string& name, float value) const
{
	GL_CHECK(glUniform1f(getUniformLocation(name), value));
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	GL_CHECK(glUniform2fv(getUniformLocation(name), 1, &value[0]));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	GL_CHECK(glUniform3fv(getUniformLocation(name), 1, &value[0]));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	GL_CHECK(glUniform4fv(getUniformLocation(name), 1, &value[0]));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
	GL_CHECK(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]));
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
	GL_CHECK(glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]));
}


void Shader::deleteShader()
{
	if (mShaderID != 0)
	{
		LOG_DEBUG(logging::gGraphicsLogger, "Deleting shader program (ID: {})", mShaderID);
		GL_CHECK(glDeleteProgram(mShaderID));
		mShaderID = 0;
	}
}

std::string Shader::loadShaderFile(std::string_view filePath)
{
	std::string code;
	std::ifstream file;

	// Open the file and check if it was successful
	file.open(filePath);
	ASSERT_LOG(logging::gGraphicsLogger, file.is_open(), "Failed to open shader file: {}", filePath);

	// Set exception mask to not throw on badbit (we'll handle errors manually)
	file.exceptions(std::ifstream::goodbit);

	// Read the file content
	std::stringstream stream;
	stream << file.rdbuf();

	// Check for any read errors
	ASSERT_LOG(logging::gGraphicsLogger, !file.bad(), "Error occurred while reading shader file: {}", filePath);

	code = stream.str();
	// Verify we actually got content
	ASSERT_LOG(logging::gGraphicsLogger, !code.empty(), "Shader file is empty: {}", filePath);

	file.close();
	return code;
}

unsigned int Shader::compileShader(unsigned int type, std::string_view source, std::string_view shaderPath)
{
	// Get shader type as string for logging
	std::string typeStr;
	switch (type)
	{
	case GL_VERTEX_SHADER: typeStr = "VERTEX"; break;
	case GL_FRAGMENT_SHADER: typeStr = "FRAGMENT"; break;
	case GL_GEOMETRY_SHADER: typeStr = "GEOMETRY"; break;
	case GL_COMPUTE_SHADER: typeStr = "COMPUTE"; break;
	default: typeStr = "UNKNOWN"; break;
	}

	LOG_DEBUG(logging::gGraphicsLogger, "Compiling {} shader from {}", typeStr, shaderPath);

	unsigned int shaderId = glCreateShader(type);
	ASSERT_LOG(logging::gGraphicsLogger, shaderId != 0, "Failed to create shader object");

	const char* src = source.data();
	GL_CHECK(glShaderSource(shaderId, 1, &src, nullptr));
	GL_CHECK(glCompileShader(shaderId));

	// Check for compilation errors
	checkCompileError(shaderId, typeStr, std::string(shaderPath));

	return shaderId;
}

void Shader::checkCompileError(unsigned int shader, const std::string& type, const std::string& filePath)
{
	int success;
	char infoLog[1024];

	if (type != "PROGRAM")
	{
		GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
		if (!success)
		{
			GL_CHECK(glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog));
			std::string errorMessage = "Shader compilation error (" + type + "): " + infoLog;

			if (!filePath.empty())
			{
				errorMessage += " [File: " + filePath + "]";
			}

			ASSERT_LOG(logging::gGraphicsLogger, false, "{}", errorMessage);
		}
	}
	else
	{
		GL_CHECK(glGetProgramiv(shader, GL_LINK_STATUS, &success));
		if (!success)
		{
			GL_CHECK(glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog));
			std::string errorMessage = "Shader program linking error: " + std::string(infoLog);

			if (!filePath.empty())
			{
				errorMessage += " [Files: " + filePath + "]";
			}

			ASSERT_LOG(logging::gGraphicsLogger, false, "{}", errorMessage);
		}
	}
}

int Shader::getUniformLocation(const std::string& name) const
{
	// Try to find in the cache first for performance
	auto it = mUniformLocationCache.find(name);
	if (it != mUniformLocationCache.end())
	{
		return it->second;
	}

	// Not found in cache, query OpenGL
	int location = glGetUniformLocation(mShaderID, name.c_str());

	if (location == -1)
	{
		LOG_WARN(logging::gGraphicsLogger, "Uniform '{}' not found in shader program (ID: {})", name, mShaderID);
	}

	// Cache the result (even if -1, to avoid repeated lookups for non-existent uniforms)
	mUniformLocationCache[name] = location;
	return location;
}
