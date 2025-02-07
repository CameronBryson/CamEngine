#pragma once

#include "Mesh.hpp"
#include "OpenGLMesh.hpp"
#include "Vertex.hpp"
#include <memory>
#include <vector>

class MeshFactory
{
public:
    static std::shared_ptr<Mesh> createCube();
	static std::shared_ptr<Mesh> createQuad();
};
