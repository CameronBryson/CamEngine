#include "pch.hpp"
#include "Material.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "TextureSlots.hpp"
#include "OpenGLUtil.hpp"
#include "Engine/Util/Logging.hpp"



Material::Material()
{
	// Optionally log creation
	// LOG_TRACE(logging::gGraphicsLogger, "Material created.");
}

// Destructor (implicit default is fine)
// Material::~Material() {
//     LOG_TRACE(logging::gGraphicsLogger, "Destroying Material '{}'.", mName);
// }

void Material::bind(const Shader& shader) const
{
	LOG_TRACE(logging::gGraphicsLogger, "Binding Material '{}'.", mName);
	shader.use();
	// unbind(); // Consider if unbinding everything here first is necessary or efficient.
			 // It might be better to only unbind specific slots if textures change.

	// Base Properties
	shader.setVec4("material.albedo", mAlbedo);
	shader.setFloat("material.opacity", mOpacity);

	// PBR Properties
	shader.setFloat("material.metallic", mMetallic);
	shader.setFloat("material.roughness", mRoughness);

	// Emission Properties
	shader.setVec3("material.emissiveColor", mEmissiveColor);
	shader.setFloat("material.emissiveIntensity", mEmissiveIntensity);

	// Reflective Properties
	shader.setFloat("material.reflectivity", mReflectivity);


	int currentTextureUnit = MaterialSlots::ALBEDO; // Start with the first slot

	// Helper lambda to bind texture and set uniforms
	auto bindTexture = [&](const std::shared_ptr<Texture>& tex, const char* uniformName, const char* hasUniformName, int slot)
		{
			if (tex)
			{
				shader.setInt(uniformName, slot);
				shader.setBool(hasUniformName, true);
				tex->bind(slot);
				// LOG_TRACE(logging::gGraphicsLogger, "Material '{}': Bound texture '{}' to slot {}", mName, tex->getName(), slot); // Requires Texture::getName()
				return true;
			}
			else
			{
				shader.setBool(hasUniformName, false);
				// Optionally unbind the slot if necessary, though often not needed if shaders check the 'has' flag
				// GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
				// GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); 
				return false;
			}
		};

	bindTexture(mAlbedoTexture, "material.albedoMap", "material.hasAlbedoMap", currentTextureUnit++);
	bindTexture(mNormalTexture, "material.normalMap", "material.hasNormalMap", currentTextureUnit++);
	bindTexture(mMetallicTexture, "material.metallicMap", "material.hasMetallicMap", currentTextureUnit++);
	bindTexture(mRoughnessTexture, "material.roughnessMap", "material.hasRoughnessMap", currentTextureUnit++);
	bindTexture(mAOTexture, "material.AOMap", "material.hasAOMap", currentTextureUnit++);
	bindTexture(mEmissiveTexture, "material.emissiveMap", "material.hasEmissiveMap", currentTextureUnit++);
	bindTexture(mMetalRoughTexture, "material.metalRoughMap", "material.hasMetalRoughMap", currentTextureUnit++);
	bindTexture(mOpacityTexture, "material.opacityMap", "material.hasOpacityMap", currentTextureUnit++);
}

void Material::unbind() const
{
	LOG_TRACE(logging::gGraphicsLogger, "Unbinding Material '{}'.", mName);

	int currentTextureUnit = MaterialSlots::ALBEDO;

	// Define the textures in the order they were bound
	const std::vector<std::shared_ptr<Texture>> textures = {
		mAlbedoTexture, mNormalTexture, mMetallicTexture, mRoughnessTexture,
		mAOTexture, mEmissiveTexture, mMetalRoughTexture, mOpacityTexture
	};

	// Unbind textures if they exist
	for (const auto& tex : textures)
	{
		if (tex)
		{
			// LOG_TRACE(logging::gGraphicsLogger, "Material '{}': Unbinding texture '{}' from slot {}", mName, tex->getName(), currentTextureUnit); // Requires Texture::getName()
			tex->unbind(currentTextureUnit);
		}
		currentTextureUnit++;
	}

	// Reset active texture unit to default
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
}


