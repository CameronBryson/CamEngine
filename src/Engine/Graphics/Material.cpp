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

void Material::bind(Shader& bindShader)
{
	LOG_TRACE(logging::gGraphicsLogger, "Binding Material '{}'.", mName);
	bindShader.use();
	// unbind(); // Consider if unbinding everything here first is necessary or efficient.
             // It might be better to only unbind specific slots if textures change.

	// Base Properties
	bindShader.setVec4("material.albedo", mAlbedo);
	bindShader.setFloat("material.opacity", mOpacity);

	// PBR Properties
	bindShader.setFloat("material.metallic", mMetallic);
	bindShader.setFloat("material.roughness", mRoughness);

	// Emission Properties
	bindShader.setVec3("material.emissiveColor", mEmissiveColor);
	bindShader.setFloat("material.emissiveIntensity", mEmissiveIntensity);

	// Reflective Properties
	bindShader.setFloat("material.reflectivity", mReflectivity);

	// Displacement Properties
	bindShader.setFloat("material.displacementScale", mDisplacementScale);

	int currentTextureUnit = MaterialSlots::ALBEDO; // Start with the first slot

	// Helper lambda to bind texture and set uniforms
	auto bindTexture = [&](const std::shared_ptr<Texture>& tex, const char* uniformName, const char* hasUniformName, int slot) {
		if (tex) {
			bindShader.setInt(uniformName, slot);
			bindShader.setBool(hasUniformName, true);
			tex->bind(slot);
			// LOG_TRACE(logging::gGraphicsLogger, "Material '{}': Bound texture '{}' to slot {}", mName, tex->getName(), slot); // Requires Texture::getName()
			return true;
		} else {
			bindShader.setBool(hasUniformName, false);
			// Optionally unbind the slot if necessary, though often not needed if shaders check the 'has' flag
			// GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
			// GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); 
			return false;
		}
	};

	bindTexture(mAlbedoTexture,      "material.albedoMap",       "material.hasAlbedoMap",       currentTextureUnit++);
	bindTexture(mNormalTexture,      "material.normalMap",       "material.hasNormalMap",       currentTextureUnit++);
	bindTexture(mMetallicTexture,    "material.metallicMap",     "material.hasMetallicMap",     currentTextureUnit++);
	bindTexture(mRoughnessTexture,   "material.roughnessMap",    "material.hasRoughnessMap",    currentTextureUnit++);
	bindTexture(mAOTexture,          "material.aoMap",           "material.hasAOMap",           currentTextureUnit++);
	bindTexture(mEmissiveTexture,    "material.emissiveMap",     "material.hasEmissiveMap",     currentTextureUnit++);
	bindTexture(mMetalRoughTexture,  "material.metalRoughMap",   "material.hasMetalRoughMap",   currentTextureUnit++);
	bindTexture(mOpacityTexture,     "material.opacityMap",      "material.hasOpacityMap",      currentTextureUnit++);
	bindTexture(mDisplacementTexture,"material.displacementMap", "material.hasDisplacementMap", currentTextureUnit++);
}

void Material::unbind()
{
	LOG_TRACE(logging::gGraphicsLogger, "Unbinding Material '{}'.", mName);

	int currentTextureUnit = MaterialSlots::ALBEDO;

	// Define the textures in the order they were bound
	const std::vector<std::shared_ptr<Texture>> textures = {
		mAlbedoTexture, mNormalTexture, mMetallicTexture, mRoughnessTexture,
		mAOTexture, mEmissiveTexture, mMetalRoughTexture, mOpacityTexture,
		mDisplacementTexture
	};

	// Unbind textures if they exist
	for (const auto& tex : textures) {
		if (tex) {
			// LOG_TRACE(logging::gGraphicsLogger, "Material '{}': Unbinding texture '{}' from slot {}", mName, tex->getName(), currentTextureUnit); // Requires Texture::getName()
			tex->unbind(currentTextureUnit);
		}
		currentTextureUnit++;
	}

	// Reset active texture unit to default
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
}


