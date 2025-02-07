
#include "pch.hpp"
#include "OpenGLMaterial.hpp"

#include "OpenGLShader.hpp"
#include "OpenGLTexture2D.hpp"
#include "TextureSlots.hpp"

// Define texture unit offsets for clarity and maintainability


OpenGLMaterial::OpenGLMaterial(const glm::vec4& albedo,
                               float metallic,
                               float roughness,
                               float AO,
                               std::shared_ptr<Texture> albedoTexture,
                               std::shared_ptr<Texture> normalTexture,
                               std::shared_ptr<Texture> metallicTexture,
                               std::shared_ptr<Texture> roughnessTexture,
                               std::shared_ptr<Texture> AOTexture,
                               std::shared_ptr<Texture> emissiveTexture,
	std::shared_ptr<Texture> metalRoughTexture)
{
	this->albedo = albedo;
	this->metallic = metallic;
	this->roughness = roughness;
	this->AO = AO;
	this->albedoTexture = std::move(albedoTexture);
	this->normalTexture = std::move(normalTexture);
	this->metallicTexture = std::move(metallicTexture);
	this->roughnessTexture = std::move(roughnessTexture);
	this->AOTexture = std::move(AOTexture);
	this->emissiveTexture = std::move(emissiveTexture);
	this->metalRoughTexture = std::move(metalRoughTexture);
}

void OpenGLMaterial::bind()
{
	shader->use();

	// Activate and bind Albedo Texture if available
	if (albedoTexture)
	{
		shader->setInt("material.albedoMap", TEXTURE_UNIT_ALBEDO);
		shader->setBool("material.hasAlbedoMap", true);
		albedoTexture->bind(TEXTURE_UNIT_ALBEDO);
	}
	else
	{
		shader->setBool("material.hasAlbedoMap", false);
	}

	// Activate and bind Normal Map if available
	if (normalTexture)
	{
		shader->setInt("material.normalMap", TEXTURE_UNIT_NORMAL);
		shader->setBool("material.hasNormalMap", true);
		normalTexture->bind(TEXTURE_UNIT_NORMAL);
	}
	else
	{
		shader->setBool("material.hasNormalMap", false);
	}

	// Activate and bind Metallic Map if available
	if (metallicTexture)
	{
		shader->setInt("material.metallicMap", TEXTURE_UNIT_METALLIC);
		shader->setBool("material.hasMetallicMap", true);
		metallicTexture->bind(TEXTURE_UNIT_METALLIC);
	}
	else
	{
		shader->setBool("material.hasMetallicMap", false);
	}

	// Activate and bind Roughness Map if available
	if (roughnessTexture)
	{
		shader->setInt("material.roughnessMap", TEXTURE_UNIT_ROUGHNESS);
		shader->setBool("material.hasRoughnessMap", true);
		roughnessTexture->bind(TEXTURE_UNIT_ROUGHNESS);
	}
	else
	{
		shader->setBool("material.hasRoughnessMap", false);
	}

	// Activate and bind Ambient Occlusion Map if available
	if (AOTexture)
	{
		shader->setInt("material.AOMap", TEXTURE_UNIT_AO);
		shader->setBool("material.hasAOMap", true);
		AOTexture->bind(TEXTURE_UNIT_AO);
	}
	else
	{
		shader->setBool("material.hasAOMap", false);
	}

	// Activate and bind Emissive Map if available
	if (emissiveTexture)
	{
		shader->setInt("material.emissiveMap", TEXTURE_UNIT_EMISSIVE);
		shader->setBool("material.hasEmissiveMap", true);
		emissiveTexture->bind(TEXTURE_UNIT_EMISSIVE);
	}
	else
	{
		shader->setBool("material.hasEmissiveMap", false);
	}
	// Combined MetalRough
	if (metalRoughTexture)
	{
		shader->setBool("material.hasMetalRoughMap", true);
		shader->setInt("material.metalRoughMap", TEXTURE_UNIT_METALROUGH);
		metalRoughTexture->bind(TEXTURE_UNIT_METALROUGH);
	}
	else
	{
		shader->setBool("material.hasMetalRoughMap", false);
	}

	// Set material properties in shader
	shader->setVec4("material.albedo", albedo);
	shader->setFloat("material.metallic", metallic);
	shader->setFloat("material.roughness", roughness);
	shader->setFloat("material.AO", AO);
}


void OpenGLMaterial::unbind()
{
	if (albedoTexture)      albedoTexture->unbind(TEXTURE_UNIT_ALBEDO);
	if (normalTexture)      normalTexture->unbind(TEXTURE_UNIT_NORMAL);
	if (metallicTexture)    metallicTexture->unbind(TEXTURE_UNIT_METALLIC);
	if (roughnessTexture)   roughnessTexture->unbind(TEXTURE_UNIT_ROUGHNESS);
	if (AOTexture)          AOTexture->unbind(TEXTURE_UNIT_AO);
	if (emissiveTexture)    emissiveTexture->unbind(TEXTURE_UNIT_EMISSIVE);
	if (metalRoughTexture)  metalRoughTexture->unbind(TEXTURE_UNIT_METALROUGH);

	// reset
	glActiveTexture(GL_TEXTURE0);
}

void OpenGLMaterial::setShader(std::shared_ptr<Shader> shader)
{
	this->shader = std::move(shader);
}

std::shared_ptr<Shader> OpenGLMaterial::getShader()
{
	return shader;
}

