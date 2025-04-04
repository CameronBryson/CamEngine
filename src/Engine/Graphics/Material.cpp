#include "pch.hpp"
#include "Material.hpp"

#include "Shader.hpp"
#include "Texture2D.hpp"
#include "TextureSlots.hpp"
#include <OpenGLUtil.hpp>

// Define texture unit offsets for clarity and maintainability


Material::Material()
{
}

void Material::bind(std::shared_ptr<Shader> bindShader)
{
    bindShader->use();
    unbind(); // Clear previous bindings

    // Base Properties
    bindShader->setVec4("material.albedo", albedo);
    bindShader->setFloat("material.opacity", opacity);

    // PBR Properties
    bindShader->setFloat("material.metallic", metallic);
    bindShader->setFloat("material.roughness", roughness);

    // Emission Properties
    bindShader->setVec3("material.emissiveColor", emissiveColor);
    bindShader->setFloat("material.emissiveIntensity", emissiveIntensity);

    // Reflective Properties
    bindShader->setFloat("material.reflectivity", reflectivity);


    // Displacement Properties
    bindShader->setFloat("material.displacementScale", displacementScale);

    // Albedo Texture
    if (albedoTexture) {
        bindShader->setInt("material.albedoMap", MaterialSlots::ALBEDO);
        bindShader->setBool("material.hasAlbedoMap", true);
        albedoTexture->bind(MaterialSlots::ALBEDO);
    }
    else {
        bindShader->setBool("material.hasAlbedoMap", false);
    }

    // Normal Map
    if (normalTexture) {
        bindShader->setInt("material.normalMap", MaterialSlots::NORMAL);
        bindShader->setBool("material.hasNormalMap", true);
        normalTexture->bind(MaterialSlots::NORMAL);
    }
    else {
        bindShader->setBool("material.hasNormalMap", false);
    }

    // Metallic Map
    if (metallicTexture) {
        bindShader->setInt("material.metallicMap", MaterialSlots::METALLIC);
        bindShader->setBool("material.hasMetallicMap", true);
        metallicTexture->bind(MaterialSlots::METALLIC);
    }
    else {
        bindShader->setBool("material.hasMetallicMap", false);
    }

    // Roughness Map
    if (roughnessTexture) {
        bindShader->setInt("material.roughnessMap", MaterialSlots::ROUGHNESS);
        bindShader->setBool("material.hasRoughnessMap", true);
        roughnessTexture->bind(MaterialSlots::ROUGHNESS);
    }
    else {
        bindShader->setBool("material.hasRoughnessMap", false);
    }

    // AO Map
    if (AOTexture) {
        bindShader->setInt("material.aoMap", MaterialSlots::AO);
        bindShader->setBool("material.hasAOMap", true);
        AOTexture->bind(MaterialSlots::AO);
    }
    else {
        bindShader->setBool("material.hasAOMap", false);
    }

    // Emissive Map
    if (emissiveTexture) {
        bindShader->setInt("material.emissiveMap", MaterialSlots::EMISSIVE);
        bindShader->setBool("material.hasEmissiveMap", true);
        emissiveTexture->bind(MaterialSlots::EMISSIVE);
    }
    else {
        bindShader->setBool("material.hasEmissiveMap", false);
    }

    // Combined Metal-Rough Map
    if (metalRoughTexture) {
        bindShader->setInt("material.metalRoughMap", MaterialSlots::METALROUGH);
        bindShader->setBool("material.hasMetalRoughMap", true);
        metalRoughTexture->bind(MaterialSlots::METALROUGH);
    }
    else {
        bindShader->setBool("material.hasMetalRoughMap", false);
    }

    // Opacity Map
    if (opacityTexture) {
        bindShader->setInt("material.opacityMap", MaterialSlots::OPACITY);
        bindShader->setBool("material.hasOpacityMap", true);
        opacityTexture->bind(MaterialSlots::OPACITY);
    }
    else {
        bindShader->setBool("material.hasOpacityMap", false);
    }

    // Displacement Map
    if (displacementTexture) {
        bindShader->setInt("material.displacementMap", MaterialSlots::DISPLACEMENT);
        bindShader->setBool("material.hasDisplacementMap", true);
        displacementTexture->bind(MaterialSlots::DISPLACEMENT);
    }
    else {
        bindShader->setBool("material.hasDisplacementMap", false);
    }
}

void Material::unbind()
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


