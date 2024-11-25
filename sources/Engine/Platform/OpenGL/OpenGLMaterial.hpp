#pragma once
#include <string>
#include <memory>
#include <glm/vec3.hpp>
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Graphics/Material.hpp"


class OpenGLMaterial : public Material
{
public:
	explicit OpenGLMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum, const std::string& map_Ka_path, const std::string& map_Kd_path,
		const std::string& map_Ks_path, const std::string& map_Ns_path, const std::string& map_d_path, const std::string& map_bump_path);

	void bind(const Shader& shader) override;
	void unbind() override;

	glm::vec3 Ka; //Ka
	glm::vec3 Kd; //Kd
	glm::vec3 Ks; //Ks
	float Ns;
	float Ni;
	float d;
	int illum;
	std::unique_ptr<Texture> map_Ka;
	std::unique_ptr<Texture> map_Kd;
	std::unique_ptr<Texture> map_Ks;
	std::unique_ptr<Texture> map_Ns;
	std::unique_ptr<Texture> map_d;
	std::unique_ptr<Texture> map_bump;
};
