#include "pch.hpp"
#include "Mesh.hpp" 
#include "Engine/Platform/OpenGL/OpenGLMesh.hpp"



std::shared_ptr<Mesh> Mesh::createMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::shared_ptr<Material>& material)
{
	return std::make_shared<OpenGLMesh>(vertices, indices, material);
}
