#include "pch.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "Engine/Platform/OpenGL/OpenGLModel.hpp"

std::shared_ptr<Model> Model::createModel(const std::vector<std::shared_ptr<Mesh>>& meshes)
{
	return std::make_shared<OpenGLModel>(meshes);
}
