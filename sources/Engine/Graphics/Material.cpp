#include "Material.hpp"

#include "Engine/Util/OpenGLUtil.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"
Material::Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum, const std::string& map_Ka_path, const std::string& map_Kd_path,
	const std::string& map_Ks_path, const std::string& map_Ns_path, const std::string& map_d_path, const std::string& map_bump_path)
	: Ka(Ka), Kd(Kd), Ks(Ks), Ns(Ns),Ni(Ni), d(d), illum(illum)
{
	if( ! map_Ka_path.empty() )
	{
		map_Ka = std::make_unique<Texture>(map_Ka_path);
	}
	if( ! map_Kd_path.empty() )
	{
		map_Kd = std::make_unique<Texture>(map_Kd_path);
	}
	if( ! map_Ks_path.empty() )
	{
		map_Ks = std::make_unique<Texture>(map_Ks_path);
	}
	if( ! map_Ns_path.empty() )
	{
		map_Ns = std::make_unique<Texture>(map_Ns_path);
	}
	if( ! map_d_path.empty() )
	{
		map_d = std::make_unique<Texture>(map_d_path);
	}
	if( ! map_bump_path.empty() )
	{
		map_bump = std::make_unique<Texture>(map_bump_path);
	}
}
void Material::bind(const ShaderProgram& shader)
{
	OpenGlUtil::bindMaterial(shader, *this);
}
void Material::unbind()
{
	OpenGlUtil::unbindMaterial(*this);
}
