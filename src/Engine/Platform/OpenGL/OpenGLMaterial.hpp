#pragma once
#include <string>
#include <memory>
#include <glm/vec4.hpp>
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Graphics/Material.hpp"

class Texture;
class OpenGLMaterial : public Material
{
public:
	explicit OpenGLMaterial(const glm::vec4& albedo,
	                        float metallic,
	                        float roughness,
	                        float AO,
	                        std::shared_ptr<Texture> albedoTexture,
	                        std::shared_ptr<Texture> normalTexture,
	                        std::shared_ptr<Texture> metallicTexture,
	                        std::shared_ptr<Texture> roughNessTexture,
	                        std::shared_ptr<Texture> AOTexture,
	                        std::shared_ptr<Texture> emissiveTexture,
	std::shared_ptr<Texture> metalRoughTexture =nullptr );

	void bind(const Shader& shader) override;
	void unbind() override;

	glm::vec4 albedo;
	float metallic;
	float roughness;
	float AO;

	std::shared_ptr<Texture> albedoTexture;
	std::shared_ptr<Texture> normalTexture;
	std::shared_ptr<Texture> metallicTexture;
	std::shared_ptr<Texture> roughnessTexture;
	std::shared_ptr<Texture> AOTexture;
	std::shared_ptr<Texture> emissiveTexture;
	std::shared_ptr<Texture> metalRoughTexture;
};

