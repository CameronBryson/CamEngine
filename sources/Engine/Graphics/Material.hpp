#pragma once
#include <string>
#include <memory>
#include <glm/vec3.hpp>
class Shader;
class Texture;

class Material
{
public:
	virtual ~Material() = default;
	virtual void bind(const Shader& shader) = 0;
	virtual void unbind() = 0;

	static std::shared_ptr<Material> createMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 emmisive, float shininess,float opicalDensity, float transparency, int illum, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
	                                                std::shared_ptr<Texture> specularMap,
	                                                std::shared_ptr<Texture> normalMap,
	                                                std::shared_ptr<Texture> roughnessMap);
};
