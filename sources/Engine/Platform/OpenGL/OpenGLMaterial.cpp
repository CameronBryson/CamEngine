#include "OpenGLMaterial.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLShader.hpp"

OpenGLMaterial::OpenGLMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float Ns, float Ni, float d, int illum,
	const std::string& map_Ka_path, const std::string& map_Kd_path, const std::string& map_Ks_path,
	const std::string& map_Ns_path, const std::string& map_d_path, const std::string& map_bump_path) : Ka(Ka), Kd(Kd), Ks(Ks), Ns(Ns), Ni(Ni), d(d), illum(illum)
{
	if (!map_Ka_path.empty())
	{
		map_Ka = std::make_unique<OpenGLTexture>(map_Ka_path);
	}
	if (!map_Kd_path.empty())
	{
		map_Kd = std::make_unique<OpenGLTexture>(map_Kd_path);
	}
	if (!map_Ks_path.empty())
	{
		map_Ks = std::make_unique<OpenGLTexture>(map_Ks_path);
	}
	if (!map_Ns_path.empty())
	{
		map_Ns = std::make_unique<OpenGLTexture>(map_Ns_path);
	}
	if (!map_d_path.empty())
	{
		map_d = std::make_unique<OpenGLTexture>(map_d_path);
	}
	if (!map_bump_path.empty())
	{
		map_bump = std::make_unique<OpenGLTexture>(map_bump_path);
	}
}

void OpenGLMaterial::bind(const Shader& shader)
{
	// Bind Ambient Texture (map_Ka)
	if (map_Ka)
	{
		shader.setInt("material.ambient", 0); // Bind texture unit 0 to ambient map
		glActiveTexture(GL_TEXTURE0);
		map_Ka->bind();
	}
	else
	{
		shader.setVec3("material.ambient", Ka); // Fallback to ambient color
	}

	// Bind Diffuse Texture (map_Kd)
	if (map_Kd)
	{
		shader.setInt("material.diffuse", 1); // Bind texture unit 1 to diffuse map
		glActiveTexture(GL_TEXTURE1);
		map_Kd->bind();
	}
	else
	{
		shader.setVec3("material.diffuse", Kd); // Fallback to diffuse color
	}

	// Bind Specular Texture (map_Ks)
	if (map_Ks)
	{
		shader.setInt("material.specular", 2); // Bind texture unit 2 to specular map
		glActiveTexture(GL_TEXTURE2);
		map_Ks->bind();
	}
	else
	{
		shader.setVec3("material.specular", Ks); // Fallback to specular color
	}

	// Bind Shininess Texture (map_Ns)
	if (map_Ns)
	{
		shader.setInt("material.shininessMap", 3); // Bind texture unit 3 to shininess map
		glActiveTexture(GL_TEXTURE3);
		map_Ns->bind();
	}
	else
	{
		shader.setFloat("material.shininess", Ns); // Fallback to shininess value
	}

	// Bind Transparency Texture (map_d)
	if (map_d)
	{
		shader.setInt("material.transparencyMap", 4); // Bind texture unit 4 to transparency map
		glActiveTexture(GL_TEXTURE4);
		map_d->bind();
		shader.setFloat("material.transparency", d); // Set transparency factor (d)
	}
	else
	{
		shader.setFloat("material.transparency", d); // Fallback to transparency value
	}

	// Bind Bump Map (map_bump)
	if (map_bump)
	{
		shader.setInt("material.bumpMap", 5); // Bind texture unit 5 to bump map
		glActiveTexture(GL_TEXTURE5);
		map_bump->bind();
	}

	// Set other material properties
	shader.setVec3("material.ambientColor", Ka); // Set the ambient color (Ka)
	shader.setVec3("material.diffuseColor", Kd); // Set the diffuse color (Kd)
	shader.setVec3("material.specularColor", Ks); // Set the specular color (Ks)
	shader.setFloat("material.shininess", Ns); // Set shininess (Ns)
}

void OpenGLMaterial::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
