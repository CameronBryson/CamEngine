#pragma once
#include <memory>
#include <glm/vec4.hpp>
#include "Material.hpp"
class Texture;
class MaterialPBR : public Material
{
public:
	static std::shared_ptr<MaterialPBR> createMaterialPBR(const glm::vec4& albedo,
		float metallic,
		float roughness,
		float AO,
		std::shared_ptr<Texture> albedoTexture,
		std::shared_ptr<Texture> normalTexture,
		std::shared_ptr<Texture> metallicTexture,
		std::shared_ptr<Texture> roughNessTexture,
		std::shared_ptr<Texture> AOTexture,
		std::shared_ptr<Texture> emissiveTexture,
		std::shared_ptr<Texture> metalRoughTexture);
};

