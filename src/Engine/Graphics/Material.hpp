#pragma once
#include <string>
#include <memory>
#include <Shader.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

class Texture;
class Material
{
public:
    Material();
    void bind(std::shared_ptr<Shader> shader) ;
    void unbind() ;

    // Implement all getters and setters
    void setShader(std::shared_ptr<Shader> shader)  { this->shader = shader; }
    std::shared_ptr<Shader> getShader()  { return shader; }

    void setName(const std::string& name)  { mName = name; }
    const std::string& getName() const  { return mName; }

    // Base material properties
    void setAlbedo(const glm::vec4& color)  { albedo = color; }
    glm::vec4 getAlbedo() const  { return albedo; }
    void setOpacity(float value)  { opacity = value; }
    float getOpacity() const  { return opacity; }

    // PBR properties
    void setMetallic(float value)  { metallic = value; }
    float getMetallic() const  { return metallic; }
    void setRoughness(float value)  { roughness = value; }
    float getRoughness() const  { return roughness; }

    // Emission properties
    void setEmissiveColor(const glm::vec3& color)  { emissiveColor = color; }
    glm::vec3 getEmissiveColor() const  { return emissiveColor; }
    void setEmissiveIntensity(float value)  { emissiveIntensity = value; }
    float getEmissiveIntensity() const  { return emissiveIntensity; }

    // Displacement properties
    void setDisplacementScale(float value)  { displacementScale = value; }
    float getDisplacementScale() const  { return displacementScale; }

    //Reflective properties
    void setReflectivity(float value)  { reflectivity = value; }
    float getReflectivity() const  { return reflectivity; }

    // Textures
    void setAlbedoTexture(std::shared_ptr<Texture> texture)  { albedoTexture = texture; }
    std::shared_ptr<Texture> getAlbedoTexture() const  { return albedoTexture; }
    void setNormalTexture(std::shared_ptr<Texture> texture)  { normalTexture = texture; }
    std::shared_ptr<Texture> getNormalTexture() const  { return normalTexture; }
    void setMetallicTexture(std::shared_ptr<Texture> texture)  { metallicTexture = texture; }
    std::shared_ptr<Texture> getMetallicTexture() const  { return metallicTexture; }
    void setRoughnessTexture(std::shared_ptr<Texture> texture)  { roughnessTexture = texture; }
    std::shared_ptr<Texture> getRoughnessTexture() const  { return roughnessTexture; }
    void setAOTexture(std::shared_ptr<Texture> texture)  { AOTexture = texture; }
    std::shared_ptr<Texture> getAOTexture() const  { return AOTexture; }
    void setEmissiveTexture(std::shared_ptr<Texture> texture)  { emissiveTexture = texture; }
    std::shared_ptr<Texture> getEmissiveTexture() const  { return emissiveTexture; }
    void setMetalRoughTexture(std::shared_ptr<Texture> texture)  { metalRoughTexture = texture; }
    std::shared_ptr<Texture> getMetalRoughTexture() const  { return metalRoughTexture; }
    void setOpacityTexture(std::shared_ptr<Texture> texture)  { opacityTexture = texture; }
    std::shared_ptr<Texture> getOpacityTexture() const  { return opacityTexture; }
    void setDisplacementTexture(std::shared_ptr<Texture> texture)  { displacementTexture = texture; }
    std::shared_ptr<Texture> getDisplacementTexture() const  { return displacementTexture; }

private:
    // Move all properties to private section
    glm::vec4 albedo = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    float opacity = 1.0f;
    float metallic = 0.0f;
    float roughness = 0.5f;
    glm::vec3 emissiveColor = glm::vec3(0.0f);
    float emissiveIntensity = 0.0f;
    float displacementScale = 0.1f;
    float reflectivity = 0.2f;
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



