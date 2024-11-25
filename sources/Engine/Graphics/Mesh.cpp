#include "Mesh.hpp" 
#include "Engine/Platform/OpenGL/OpenGLMesh.hpp"


std::shared_ptr<Mesh> Mesh::createMesh(const std::vector<Vertex>& vertices, const std::string& material_name)
{
	return std::make_shared<OpenGLMesh>(vertices, material_name);
}
