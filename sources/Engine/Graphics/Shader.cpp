#include "Shader.hpp"

#include "Engine/Platform/OpenGL/OpenGLShader.hpp"

std::shared_ptr<Shader> Shader::createShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	return std::make_shared<OpenGLShader>(vertexPath, fragmentPath);
}
