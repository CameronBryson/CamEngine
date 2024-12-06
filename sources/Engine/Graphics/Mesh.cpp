#include "Engine/pch.hpp"
#include "Mesh.hpp" 
#include "Engine/Platform/OpenGL/OpenGLMesh.hpp"


std::shared_ptr<Mesh> Mesh::createMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::string& material_name)
{
	return std::make_shared<OpenGLMesh>(vertices, indices, material_name);
}
