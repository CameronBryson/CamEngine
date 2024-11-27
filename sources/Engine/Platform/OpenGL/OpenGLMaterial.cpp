#include "OpenGLMaterial.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLShader.hpp"
// Define texture unit offsets for clarity and maintainability
constexpr unsigned int TEXTURE_UNIT_AMBIENT = 0;
constexpr unsigned int TEXTURE_UNIT_DIFFUSE = 1;
constexpr unsigned int TEXTURE_UNIT_SPECULAR = 2;
constexpr unsigned int TEXTURE_UNIT_NORMAL = 3;

OpenGLMaterial::OpenGLMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, float transparency, std::shared_ptr<Texture> ambientMap, std::shared_ptr<Texture> diffuseMap,
	std::shared_ptr<Texture> specularMap, std::shared_ptr<Texture> normalMap)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
	this->transparency = transparency;
	this->ambientMap = std::move(ambientMap);
	this->diffuseMap = std::move(diffuseMap);
	this->specularMap = std::move(specularMap);
	this->normalMap = std::move(normalMap);
}

void OpenGLMaterial::bind(const Shader& shader)
{
	shader.use();
	// Activate and bind Ambient Texture (map_Ka) if available
	if (ambientMap)
	{
		shader.setInt("material.ambientMap", TEXTURE_UNIT_AMBIENT);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_AMBIENT);
		ambientMap->bind();
	}
	// Activate and bind Diffuse Texture (map_Kd) if available
	if (diffuseMap)
	{
		shader.setInt("material.diffuseMap", TEXTURE_UNIT_DIFFUSE);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_DIFFUSE);
		diffuseMap->bind();
	}
	// Activate and bind Specular Texture (map_Ks) if available
	if (specularMap)
	{
		shader.setInt("material.specularMap", TEXTURE_UNIT_SPECULAR);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_SPECULAR);
		specularMap->bind();
	}
	// Activate and bind Normal/Bump Map (map_bump) if available
	if (normalMap)
	{
		shader.setInt("material.normalMap", TEXTURE_UNIT_NORMAL);
		glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_NORMAL);
		normalMap->bind();
	}

	// Set material properties in shader
	shader.setVec3("material.ambientColor", ambient);     // Set the ambient color (Ka)
	shader.setVec3("material.diffuseColor", diffuse);     // Set the diffuse color (Kd)
	shader.setVec3("material.specularColor", specular);   // Set the specular color (Ks)
	shader.setFloat("material.shininess", shininess);     // Set shininess (Ns)
	shader.setFloat("material.transparency", transparency); // Set transparency (d)
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

	// Reset active texture to default
	glActiveTexture(GL_TEXTURE0);
}
