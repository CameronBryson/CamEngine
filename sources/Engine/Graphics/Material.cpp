#include "Material.hpp"

#include "Engine/Platform/OpenGL/OpenGLMaterial.hpp"


std::shared_ptr<Material> Material::createMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, float transparency, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
	std::shared_ptr<Texture> specularMap, std::shared_ptr<Texture> normalMap)
{
	return std::make_shared<OpenGLMaterial>(ambient, diffuse, specular, shininess, transparency, ambientMap, diffuseMap, specularMap, normalMap);
}
