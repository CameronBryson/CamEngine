#include "pch.hpp"
#include "MaterialPBR.hpp"

std::shared_ptr<MaterialPBR> MaterialPBR::createMaterialPBR(const glm::vec4& albedo, float metallic, float roughness, float AO, std::shared_ptr<Texture> albedoTexture, std::shared_ptr<Texture> normalTexture, std::shared_ptr<Texture> metallicTexture, std::shared_ptr<Texture> roughNessTexture, std::shared_ptr<Texture> AOTexture, std::shared_ptr<Texture> emissiveTexture, std::shared_ptr<Texture> metalRoughTexture)
{
	return std::shared_ptr<MaterialPBR>();
}
