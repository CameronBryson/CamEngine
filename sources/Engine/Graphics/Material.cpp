#include "Material.hpp"

#include "Engine/Platform/OpenGL/OpenGLMaterial.hpp"


std::shared_ptr<Material> Material::createMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,glm::vec3 emmisive, float shininess, float opticalDensity, float transparency, int illum, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
                                                   std::shared_ptr<Texture> specularMap,
                                                   std::shared_ptr<Texture> normalMap,
                                                   std::shared_ptr<Texture> roughnessMap)
{
	return std::make_shared<OpenGLMaterial>(ambient, diffuse, specular, emmisive, shininess,opticalDensity, transparency,illum, ambientMap,
	                                        diffuseMap, specularMap, normalMap, roughnessMap);
}
