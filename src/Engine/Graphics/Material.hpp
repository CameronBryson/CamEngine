#pragma once
#include <string>
#include <memory>
#include <glm/vec4.hpp>
class Shader;
class Texture;

class Material
{
public:
	virtual ~Material() = default;
	virtual void bind(const Shader& shader) = 0;
	virtual void unbind() = 0;

	static std::shared_ptr<Material> createMaterial(const glm::vec4& albedo,
	                        float metallic,
	                        float roughness,
	                        float AO,
	                        std::shared_ptr<Texture> albedoTexture,
	                        std::shared_ptr<Texture> normalTexture,
	                        std::shared_ptr<Texture> metallicTexture,
	                        std::shared_ptr<Texture> roughNessTexture,
	                        std::shared_ptr<Texture> AOTexture,
	                        std::shared_ptr<Texture> emissiveTexture);
};
