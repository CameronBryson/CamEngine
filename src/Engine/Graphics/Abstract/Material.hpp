#pragma once
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include "Shader.hpp"
class Texture;
class Material
{
public:
	virtual ~Material() = default;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setShader(std::shared_ptr<Shader> shader) = 0;
	virtual std::shared_ptr<Shader> getShader() = 0;
	static std::shared_ptr<Material> createMaterial(const glm::vec4& albedo,
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
