#pragma once
#include <string>
#include <memory>
#include <glm/vec3.hpp>
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Graphics/Material.hpp"


class OpenGLMaterial : public Material
{
public:
	explicit OpenGLMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 emmisive, float shininess,float opticalDesnity, float transparency, int illum, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
	                        std::shared_ptr<Texture> specularMap,
	                        std::shared_ptr<Texture> normalMap,
	                        std::shared_ptr<Texture> roughnessMap);

	void bind(const Shader& shader) override;
	void unbind() override;

	glm::vec3 ambient; //Ka
	glm::vec3 diffuse; //Kd
	glm::vec3 specular; //Ks
	glm::vec3 emisive; // Ke
	float shininess;
	float opticalDensity;
	float transparency;
	int illum;
	std::shared_ptr<Texture> ambientMap;
	std::shared_ptr<Texture> diffuseMap;
	std::shared_ptr<Texture> specularMap;
	std::shared_ptr<Texture> normalMap;
	std::shared_ptr<Texture> roughnessMap;
};

