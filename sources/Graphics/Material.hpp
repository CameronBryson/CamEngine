#pragma once
#include <string>
#include <memory>

#include "OpenGLUtil.hpp"

class Texture;
class ShaderProgram;

class Material
{
public:
	Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum, const std::string& map_Ka_path, const std::string& map_Kd_path,
	const std::string& map_Ks_path, const std::string& map_Ns_path, const std::string& map_d_path, const std::string& map_bump_path);

  void bind(const ShaderProgram& shader);
  void unbind();

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
