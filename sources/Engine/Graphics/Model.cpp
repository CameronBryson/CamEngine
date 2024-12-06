#include "Engine/pch.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "Engine/Platform/OpenGL/OpenGLModel.hpp"

std::shared_ptr<Model> Model::createModel(const std::vector<std::string>& mesh_names)
{
	return std::make_shared<OpenGLModel>(mesh_names);
}
