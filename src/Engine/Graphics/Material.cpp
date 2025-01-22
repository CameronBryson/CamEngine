
#include "pch.hpp"
#include "Material.hpp"

#include "Engine/Platform/OpenGL/OpenGLMaterial.hpp"


std::shared_ptr<Material> Material::createMaterial(const glm::vec4& albedo,
	                        float metallic,
	                        float roughness,
	                        float AO,
	                        std::shared_ptr<Texture> albedoTexture,
	                        std::shared_ptr<Texture> normalTexture,
	                        std::shared_ptr<Texture> metallicTexture,
	                        std::shared_ptr<Texture> roughNessTexture,
	                        std::shared_ptr<Texture> AOTexture,
	                        std::shared_ptr<Texture> emissiveTexture)
{
	return std::make_shared<OpenGLMaterial>(albedo,metallic,roughness,AO, albedoTexture, normalTexture, metallicTexture,roughNessTexture,AOTexture,emissiveTexture);
}
