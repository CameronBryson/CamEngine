#pragma once
#include <string>
#include <memory>
#include <glm/vec3.hpp>
class Shader;

class Material
{
public:
	virtual ~Material() = default;
	virtual void bind(const Shader& shader) = 0;
	virtual void unbind() = 0;

	static std::shared_ptr<Material> createMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum, const std::string& map_Ka_path, const std::string& map_Kd_path,
		const std::string& map_Ks_path, const std::string& map_Ns_path, const std::string& map_d_path, const std::string& map_bump_path);
};
