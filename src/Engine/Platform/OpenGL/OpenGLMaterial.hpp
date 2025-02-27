#pragma once
#include <string>
#include <memory>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include "Material.hpp"

class Texture;
class OpenGLMaterial : public Material
{
public:
    OpenGLMaterial();
    void bind(std::shared_ptr<Shader> shader) override;
    void unbind() override;

    // Implement all getters and setters
    void setShader(std::shared_ptr<Shader> shader) override { this->shader = shader; }
    std::shared_ptr<Shader> getShader() override { return shader; }

    void setName(const std::string& name) override { mName = name; }
    const std::string& getName() const override { return mName; }

    // Base material properties
    void setAlbedo(const glm::vec4& color) override { albedo = color; }
    glm::vec4 getAlbedo() const override { return albedo; }
    void setOpacity(float value) override { opacity = value; }
    float getOpacity() const override { return opacity; }

    // PBR properties
    void setMetallic(float value) override { metallic = value; }
    float getMetallic() const override { return metallic; }
    void setRoughness(float value) override { roughness = value; }
    float getRoughness() const override { return roughness; }

    // Emission properties
    void setEmissiveColor(const glm::vec3& color) override { emissiveColor = color; }
    glm::vec3 getEmissiveColor() const override { return emissiveColor; }
    void setEmissiveIntensity(float value) override { emissiveIntensity = value; }
    float getEmissiveIntensity() const override { return emissiveIntensity; }

    // Displacement properties
    void setDisplacementScale(float value) override { displacementScale = value; }
    float getDisplacementScale() const override { return displacementScale; }

    // Textures
    void setAlbedoTexture(std::shared_ptr<Texture> texture) override { albedoTexture = texture; }
    std::shared_ptr<Texture> getAlbedoTexture() const override { return albedoTexture; }
    void setNormalTexture(std::shared_ptr<Texture> texture) override { normalTexture = texture; }
    std::shared_ptr<Texture> getNormalTexture() const override { return normalTexture; }
    void setMetallicTexture(std::shared_ptr<Texture> texture) override { metallicTexture = texture; }
    std::shared_ptr<Texture> getMetallicTexture() const override { return metallicTexture; }
    void setRoughnessTexture(std::shared_ptr<Texture> texture) override { roughnessTexture = texture; }
    std::shared_ptr<Texture> getRoughnessTexture() const override { return roughnessTexture; }
    void setAOTexture(std::shared_ptr<Texture> texture) override { AOTexture = texture; }
    std::shared_ptr<Texture> getAOTexture() const override { return AOTexture; }
    void setEmissiveTexture(std::shared_ptr<Texture> texture) override { emissiveTexture = texture; }
    std::shared_ptr<Texture> getEmissiveTexture() const override { return emissiveTexture; }
    void setMetalRoughTexture(std::shared_ptr<Texture> texture) override { metalRoughTexture = texture; }
    std::shared_ptr<Texture> getMetalRoughTexture() const override { return metalRoughTexture; }
    void setOpacityTexture(std::shared_ptr<Texture> texture) override { opacityTexture = texture; }
    std::shared_ptr<Texture> getOpacityTexture() const override { return opacityTexture; }
    void setDisplacementTexture(std::shared_ptr<Texture> texture) override { displacementTexture = texture; }
    std::shared_ptr<Texture> getDisplacementTexture() const override { return displacementTexture; }

private:
    // Move all properties to private section
    glm::vec4 albedo = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    float opacity = 1.0f;
    float metallic = 0.0f;
    float roughness = 0.5f;
    glm::vec3 emissiveColor = glm::vec3(0.0f);
    float emissiveIntensity = 0.0f;
    float displacementScale = 0.1f;
    std::string mName;

    std::shared_ptr<Texture> albedoTexture = nullptr;
    std::shared_ptr<Texture> normalTexture = nullptr;
    std::shared_ptr<Texture> metallicTexture = nullptr;
    std::shared_ptr<Texture> roughnessTexture = nullptr;
    std::shared_ptr<Texture> AOTexture = nullptr;
    std::shared_ptr<Texture> emissiveTexture = nullptr;
    std::shared_ptr<Texture> metalRoughTexture = nullptr;
    std::shared_ptr<Texture> opacityTexture = nullptr;
    std::shared_ptr<Texture> displacementTexture = nullptr;
    std::shared_ptr<Shader> shader = nullptr;
};



