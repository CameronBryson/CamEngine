#pragma once
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include "Shader.hpp"
class Texture;
class Material
{
public:
    virtual ~Material() = default;
    virtual void bind(std::shared_ptr<Shader> shader) = 0;
    virtual void unbind() = 0;

    // Shader
    virtual void setShader(std::shared_ptr<Shader> shader) = 0;
    virtual std::shared_ptr<Shader> getShader() = 0;

    virtual void setName(const std::string& name) = 0;
    virtual const std::string& getName() const = 0;

    // Base material properties
    virtual void setAlbedo(const glm::vec4& color) = 0;
    virtual glm::vec4 getAlbedo() const = 0;
    virtual void setOpacity(float value) = 0;
    virtual float getOpacity() const = 0;

    // PBR properties
    virtual void setMetallic(float value) = 0;
    virtual float getMetallic() const = 0;
    virtual void setRoughness(float value) = 0;
    virtual float getRoughness() const = 0;

    // Emission properties
    virtual void setEmissiveColor(const glm::vec3& color) = 0;
    virtual glm::vec3 getEmissiveColor() const = 0;
    virtual void setEmissiveIntensity(float value) = 0;
    virtual float getEmissiveIntensity() const = 0;

    // Displacement properties
    virtual void setDisplacementScale(float value) = 0;
    virtual float getDisplacementScale() const = 0;
	//Reflective properties
	virtual void setReflectivity(float value) = 0;
	virtual float getReflectivity() const = 0;
    
    // Textures
    virtual void setAlbedoTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getAlbedoTexture() const = 0;
    virtual void setNormalTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getNormalTexture() const = 0;
    virtual void setMetallicTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getMetallicTexture() const = 0;
    virtual void setRoughnessTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getRoughnessTexture() const = 0;
    virtual void setAOTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getAOTexture() const = 0;
    virtual void setEmissiveTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getEmissiveTexture() const = 0;
    virtual void setMetalRoughTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getMetalRoughTexture() const = 0;
    virtual void setOpacityTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getOpacityTexture() const = 0;
    virtual void setDisplacementTexture(std::shared_ptr<Texture> texture) = 0;
    virtual std::shared_ptr<Texture> getDisplacementTexture() const = 0;

    static std::shared_ptr<Material> createMaterial();
};

