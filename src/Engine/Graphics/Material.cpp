#include "pch.hpp"
#include "Material.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "TextureSlots.hpp"
#include <OpenGLUtil.hpp>



Material::Material()
{
}

void Material::bind(std::shared_ptr<Shader> bindShader)
{
	bindShader->use();
	unbind(); // Clear previous bindings

	// Base Properties
	bindShader->setVec4("material.albedo", mAlbedo);
	bindShader->setFloat("material.opacity", mOpacity);

	// PBR Properties
	bindShader->setFloat("material.metallic", mMetallic);
	bindShader->setFloat("material.roughness", mRoughness);

	// Emission Properties
	bindShader->setVec3("material.emissiveColor", mEmissiveColor);
	bindShader->setFloat("material.emissiveIntensity", mEmissiveIntensity);

	// Reflective Properties
	bindShader->setFloat("material.reflectivity", mReflectivity);


	// Displacement Properties
	bindShader->setFloat("material.displacementScale", mDisplacementScale);

	// Albedo Texture
	if (mAlbedoTexture) {
		bindShader->setInt("material.albedoMap", MaterialSlots::ALBEDO);
		bindShader->setBool("material.hasAlbedoMap", true);
		mAlbedoTexture->bind(MaterialSlots::ALBEDO);
	}
	else {
		bindShader->setBool("material.hasAlbedoMap", false);
	}

	// Normal Map
	if (mNormalTexture) {
		bindShader->setInt("material.normalMap", MaterialSlots::NORMAL);
		bindShader->setBool("material.hasNormalMap", true);
		mNormalTexture->bind(MaterialSlots::NORMAL);
	}
	else {
		bindShader->setBool("material.hasNormalMap", false);
	}

	// Metallic Map
	if (mMetallicTexture) {
		bindShader->setInt("material.metallicMap", MaterialSlots::METALLIC);
		bindShader->setBool("material.hasMetallicMap", true);
		mMetallicTexture->bind(MaterialSlots::METALLIC);
	}
	else {
		bindShader->setBool("material.hasMetallicMap", false);
	}

	// Roughness Map
	if (mRoughnessTexture) {
		bindShader->setInt("material.roughnessMap", MaterialSlots::ROUGHNESS);
		bindShader->setBool("material.hasRoughnessMap", true);
		mRoughnessTexture->bind(MaterialSlots::ROUGHNESS);
	}
	else {
		bindShader->setBool("material.hasRoughnessMap", false);
	}

	// AO Map
	if (mAOTexture) {
		bindShader->setInt("material.aoMap", MaterialSlots::AO);
		bindShader->setBool("material.hasAOMap", true);
		mAOTexture->bind(MaterialSlots::AO);
	}
	else {
		bindShader->setBool("material.hasAOMap", false);
	}

	// Emissive Map
	if (mEmissiveTexture) {
		bindShader->setInt("material.emissiveMap", MaterialSlots::EMISSIVE);
		bindShader->setBool("material.hasEmissiveMap", true);
		mEmissiveTexture->bind(MaterialSlots::EMISSIVE);
	}
	else {
		bindShader->setBool("material.hasEmissiveMap", false);
	}

	// Combined Metal-Rough Map
	if (mMetalRoughTexture) {
		bindShader->setInt("material.metalRoughMap", MaterialSlots::METALROUGH);
		bindShader->setBool("material.hasMetalRoughMap", true);
		mMetalRoughTexture->bind(MaterialSlots::METALROUGH);
	}
	else {
		bindShader->setBool("material.hasMetalRoughMap", false);
	}

	// Opacity Map
	if (mOpacityTexture) {
		bindShader->setInt("material.opacityMap", MaterialSlots::OPACITY);
		bindShader->setBool("material.hasOpacityMap", true);
		mOpacityTexture->bind(MaterialSlots::OPACITY);
	}
	else {
		bindShader->setBool("material.hasOpacityMap", false);
	}

	// Displacement Map
	if (mDisplacementTexture) {
		bindShader->setInt("material.displacementMap", MaterialSlots::DISPLACEMENT);
		bindShader->setBool("material.hasDisplacementMap", true);
		mDisplacementTexture->bind(MaterialSlots::DISPLACEMENT);
	}
	else {
		bindShader->setBool("material.hasDisplacementMap", false);
	}
}

void Material::unbind()
{
	// Unbind all textures in reverse order
	if (mDisplacementTexture) mDisplacementTexture->unbind(MaterialSlots::DISPLACEMENT);
	if (mOpacityTexture)      mOpacityTexture->unbind(MaterialSlots::OPACITY);
	if (mMetalRoughTexture)   mMetalRoughTexture->unbind(MaterialSlots::METALROUGH);
	if (mEmissiveTexture)     mEmissiveTexture->unbind(MaterialSlots::EMISSIVE);
	if (mAOTexture)           mAOTexture->unbind(MaterialSlots::AO);
	if (mRoughnessTexture)    mRoughnessTexture->unbind(MaterialSlots::ROUGHNESS);
	if (mMetallicTexture)     mMetallicTexture->unbind(MaterialSlots::METALLIC);
	if (mNormalTexture)       mNormalTexture->unbind(MaterialSlots::NORMAL);
	if (mAlbedoTexture)       mAlbedoTexture->unbind(MaterialSlots::ALBEDO);

	// Reset active texture unit
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
}


