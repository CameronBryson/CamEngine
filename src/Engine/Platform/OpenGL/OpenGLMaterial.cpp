
#include "pch.hpp"
#include "OpenGLMaterial.hpp"

#include "OpenGLShader.hpp"
#include "OpenGLTexture2D.hpp"
#include "TextureSlots.hpp"
#include <OpenGLUtil.hpp>

// Define texture unit offsets for clarity and maintainability


OpenGLMaterial::OpenGLMaterial()
{
}

void OpenGLMaterial::bind()
{
    shader->use();
    unbind(); // Clear previous bindings

    // Base Properties
    shader->setVec4("material.albedo", albedo);
    shader->setFloat("material.opacity", opacity);

    // PBR Properties
    shader->setFloat("material.metallic", metallic);
    shader->setFloat("material.roughness", roughness);

    // Emission Properties
    shader->setVec3("material.emissiveColor", emissiveColor);
    shader->setFloat("material.emissiveIntensity", emissiveIntensity);

    // Displacement Properties
    shader->setFloat("material.displacementScale", displacementScale);

    // Albedo Texture
    if (albedoTexture) {
        shader->setInt("material.albedoMap", MaterialSlots::ALBEDO);
        shader->setBool("material.hasAlbedoMap", true);
        albedoTexture->bind(MaterialSlots::ALBEDO);
    }
    else {
        shader->setBool("material.hasAlbedoMap", false);
    }

    // Normal Map
    if (normalTexture) {
        shader->setInt("material.normalMap", MaterialSlots::NORMAL);
        shader->setBool("material.hasNormalMap", true);
        normalTexture->bind(MaterialSlots::NORMAL);
    }
    else {
        shader->setBool("material.hasNormalMap", false);
    }

    // Metallic Map
    if (metallicTexture) {
        shader->setInt("material.metallicMap", MaterialSlots::METALLIC);
        shader->setBool("material.hasMetallicMap", true);
        metallicTexture->bind(MaterialSlots::METALLIC);
    }
    else {
        shader->setBool("material.hasMetallicMap", false);
    }

    // Roughness Map
    if (roughnessTexture) {
        shader->setInt("material.roughnessMap", MaterialSlots::ROUGHNESS);
        shader->setBool("material.hasRoughnessMap", true);
        roughnessTexture->bind(MaterialSlots::ROUGHNESS);
    }
    else {
        shader->setBool("material.hasRoughnessMap", false);
    }

    // AO Map
    if (AOTexture) {
        shader->setInt("material.aoMap", MaterialSlots::AO);
        shader->setBool("material.hasAOMap", true);
        AOTexture->bind(MaterialSlots::AO);
    }
    else {
        shader->setBool("material.hasAOMap", false);
    }

    // Emissive Map
    if (emissiveTexture) {
        shader->setInt("material.emissiveMap", MaterialSlots::EMISSIVE);
        shader->setBool("material.hasEmissiveMap", true);
        emissiveTexture->bind(MaterialSlots::EMISSIVE);
    }
    else {
        shader->setBool("material.hasEmissiveMap", false);
    }

    // Combined Metal-Rough Map
    if (metalRoughTexture) {
        shader->setInt("material.metalRoughMap", MaterialSlots::METALROUGH);
        shader->setBool("material.hasMetalRoughMap", true);
        metalRoughTexture->bind(MaterialSlots::METALROUGH);
    }
    else {
        shader->setBool("material.hasMetalRoughMap", false);
    }

    // Opacity Map
    if (opacityTexture) {
        shader->setInt("material.opacityMap", MaterialSlots::OPACITY);
        shader->setBool("material.hasOpacityMap", true);
        opacityTexture->bind(MaterialSlots::OPACITY);
    }
    else {
        shader->setBool("material.hasOpacityMap", false);
    }

    // Displacement Map
    if (displacementTexture) {
        shader->setInt("material.displacementMap", MaterialSlots::DISPLACEMENT);
        shader->setBool("material.hasDisplacementMap", true);
        displacementTexture->bind(MaterialSlots::DISPLACEMENT);
    }
    else {
        shader->setBool("material.hasDisplacementMap", false);
    }
}

void OpenGLMaterial::unbind()
{
    // Unbind all textures in reverse order
    if (displacementTexture) displacementTexture->unbind(MaterialSlots::DISPLACEMENT);
    if (opacityTexture)      opacityTexture->unbind(MaterialSlots::OPACITY);
    if (metalRoughTexture)   metalRoughTexture->unbind(MaterialSlots::METALROUGH);
    if (emissiveTexture)     emissiveTexture->unbind(MaterialSlots::EMISSIVE);
    if (AOTexture)           AOTexture->unbind(MaterialSlots::AO);
    if (roughnessTexture)    roughnessTexture->unbind(MaterialSlots::ROUGHNESS);
    if (metallicTexture)     metallicTexture->unbind(MaterialSlots::METALLIC);
    if (normalTexture)       normalTexture->unbind(MaterialSlots::NORMAL);
    if (albedoTexture)       albedoTexture->unbind(MaterialSlots::ALBEDO);

    // Reset active texture unit
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
}


