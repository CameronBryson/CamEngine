#include "pch.hpp"
#include "Material.hpp"
#include "OpenGLMaterial.hpp"
std::shared_ptr<Material> Material::createMaterial()
{
	return std::make_shared<OpenGLMaterial>();
}
