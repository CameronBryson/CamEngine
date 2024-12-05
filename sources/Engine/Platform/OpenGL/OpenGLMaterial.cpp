#include "OpenGLMaterial.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLShader.hpp"
// Define texture unit offsets for clarity and maintainability
constexpr unsigned int TEXTURE_UNIT_AMBIENT = 0;
constexpr unsigned int TEXTURE_UNIT_DIFFUSE = 1;
constexpr unsigned int TEXTURE_UNIT_SPECULAR = 2;
constexpr unsigned int TEXTURE_UNIT_NORMAL = 3;
constexpr unsigned int TEXTURE_UNIT_ROUGHNESS = 4;

OpenGLMaterial::OpenGLMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 emisive,float shininess, float opticalDensity,float transparency,int illum, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
                               std::shared_ptr<Texture> specularMap,
                               std::shared_ptr<Texture> normalMap,
                               std::shared_ptr<Texture> roughnessMap)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->emisive = emisive;
	this->shininess = shininess;
	this->opticalDensity = opticalDensity;
	this->transparency = transparency;
	this->illum = illum;
	this->ambientMap = std::move(ambientMap);
	this->diffuseMap = std::move(diffuseMap);
	this->specularMap = std::move(specularMap);
	this->normalMap = std::move(normalMap);
	this->roughnessMap = std::move(roughnessMap);
}

void OpenGLMaterial::bind(const Shader& shader)
{
	shader.use();

	// Activate and bind Ambient Texture (map_Ka) if available
	if (ambientMap)
	{
		shader.setInt("material.ambientMap", TEXTURE_UNIT_AMBIENT);
		shader.setBool("material.hasAmbientMap", true);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_AMBIENT);
		ambientMap->bind();
	}
	else
	{
		shader.setBool("material.hasAmbientMap", false);
	}

	// Activate and bind Diffuse Texture (map_Kd) if available
	if (diffuseMap)
	{
		shader.setInt("material.diffuseMap", TEXTURE_UNIT_DIFFUSE);
		shader.setBool("material.hasDiffuseMap", true);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_DIFFUSE);
		diffuseMap->bind();
	}
	else
	{
		shader.setBool("material.hasDiffuseMap", false);
	}

	// Activate and bind Specular Texture (map_Ks) if available
	if (specularMap)
	{
		shader.setInt("material.specularMap", TEXTURE_UNIT_SPECULAR);
		shader.setBool("material.hasSpecularMap", true);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_SPECULAR);
		specularMap->bind();
	}
	else
	{
		shader.setBool("material.hasSpecularMap", false);
	}

	// Activate and bind Normal/Bump Map (map_bump) if available
	if (normalMap)
	{
		shader.setInt("material.normalMap", TEXTURE_UNIT_NORMAL);
		shader.setBool("material.hasNormalMap", true);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_NORMAL);
		normalMap->bind();
	}
	else
	{
		shader.setBool("material.hasNormalMap", false);
	}

	// Activate and bind Roughness Map if available
	if (roughnessMap)
	{
		shader.setInt("material.roughnessMap", TEXTURE_UNIT_ROUGHNESS);
		shader.setBool("material.hasRoughnessMap", true);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_ROUGHNESS);
		roughnessMap->bind();
	}
	else
	{
		shader.setBool("material.hasRoughnessMap", false);
	}

	// Set material properties in shader
	shader.setVec3("material.ambientColor", ambient);           // Set the ambient color (Ka)
	shader.setVec3("material.diffuseColor", diffuse);           // Set the diffuse color (Kd)
	shader.setVec3("material.specularColor", specular);         // Set the specular color (Ks)
	shader.setVec3("material.emissiveColor", emisive);          // Set the emissive color (Ke)
	shader.setFloat("material.shininess", shininess);           // Set shininess (Ns)
	shader.setFloat("material.opticalDensity", opticalDensity); // Set optical density (Ni)
	shader.setFloat("material.transparency", transparency);     // Set transparency (d)
	shader.setInt("material.illum", illum);                     // Set illumination model
}

void OpenGLMaterial::unbind()
{
	// Unbind Ambient Texture
	if (ambientMap)
	{
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_AMBIENT);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Unbind Diffuse Texture
	if (diffuseMap)
	{
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_DIFFUSE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Unbind Specular Texture
	if (specularMap)
	{
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_SPECULAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Unbind Normal/Bump Map
	if (normalMap)
	{
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_NORMAL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (roughnessMap)
	{
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_ROUGHNESS);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Reset active texture to default
	glActiveTexture(GL_TEXTURE0);
}
