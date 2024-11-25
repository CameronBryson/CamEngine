#include "Material.hpp"

#include "Engine/Platform/OpenGL/OpenGLMaterial.hpp"


std::shared_ptr<Material> Material::createMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni,
                                                   float d, int illum, const std::string& map_Ka_path, const std::string& map_Kd_path, const std::string& map_Ks_path,
                                                   const std::string& map_Ns_path, const std::string& map_d_path, const std::string& map_bump_path)
{
	return std::make_shared<OpenGLMaterial>(Ka, Kd, Ks, Ns, Ni, d, illum, map_Ka_path, map_Kd_path, map_Ks_path, map_Ns_path, map_d_path, map_bump_path);
}
