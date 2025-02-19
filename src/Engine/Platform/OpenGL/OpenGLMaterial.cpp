
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
	unbind();

	// Activate and bind Albedo Texture if available
	if (albedoTexture)
	{
		shader->setInt("material.albedoMap", MaterialSlots::ALBEDO);
		shader->setBool("material.hasAlbedoMap", true);
		albedoTexture->bind(MaterialSlots::ALBEDO);
	}
	else
	{
		shader->setBool("material.hasAlbedoMap", false);
	}

	// Activate and bind Normal Map if available
	if (normalTexture)
	{
		shader->setInt("material.normalMap", MaterialSlots::NORMAL);
		shader->setBool("material.hasNormalMap", true);
		normalTexture->bind(MaterialSlots::NORMAL);
	}
	else
	{
		shader->setBool("material.hasNormalMap", false);
	}

	// Activate and bind Metallic Map if available
	if (metallicTexture)
	{
		shader->setInt("material.metallicMap", MaterialSlots::METALLIC);
		shader->setBool("material.hasMetallicMap", true);
		metallicTexture->bind(MaterialSlots::METALLIC);
	}
	else
	{
		shader->setBool("material.hasMetallicMap", false);
	}

	// Activate and bind Roughness Map if available
	if (roughnessTexture)
	{
		shader->setInt("material.roughnessMap", MaterialSlots::ROUGHNESS);
		shader->setBool("material.hasRoughnessMap", true);
		roughnessTexture->bind(MaterialSlots::ROUGHNESS);
	}
	else
	{
		shader->setBool("material.hasRoughnessMap", false);
	}

	// Activate and bind Ambient Occlusion Map if available
	if (AOTexture)
	{
		shader->setInt("material.AOMap", MaterialSlots::AO);
		shader->setBool("material.hasAOMap", true);
		AOTexture->bind(MaterialSlots::AO);
	}
	else
	{
		shader->setBool("material.hasAOMap", false);
	}

	// Activate and bind Emissive Map if available
	if (emissiveTexture)
	{
		shader->setInt("material.emissiveMap", MaterialSlots::EMISSIVE);
		shader->setBool("material.hasEmissiveMap", true);
		emissiveTexture->bind(MaterialSlots::EMISSIVE);
	}
	else
	{
		shader->setBool("material.hasEmissiveMap", false);
	}
	// Combined MetalRough
	if (metalRoughTexture)
	{
		shader->setBool("material.hasMetalRoughMap", true);
		shader->setInt("material.metalRoughMap", MaterialSlots::METALROUGH);
		metalRoughTexture->bind(MaterialSlots::METALROUGH);
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
	if (albedoTexture)      albedoTexture->unbind(MaterialSlots::ALBEDO);
	if (normalTexture)      normalTexture->unbind(MaterialSlots::NORMAL);
	if (metallicTexture)    metallicTexture->unbind(MaterialSlots::METALLIC);
	if (roughnessTexture)   roughnessTexture->unbind(MaterialSlots::ROUGHNESS);
	if (AOTexture)          AOTexture->unbind(MaterialSlots::AO);
	if (emissiveTexture)    emissiveTexture->unbind(MaterialSlots::EMISSIVE);
	if (metalRoughTexture)  metalRoughTexture->unbind(MaterialSlots::METALROUGH);

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

