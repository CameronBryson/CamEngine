#pragma once
#include <string>
#include <memory>
#include <string_view>
#include <vector>
#include <Shader.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

class Texture;
class Shader;

class Material
{
public:
	Material();
	void bind(Shader& shader);
	void unbind();

	void setShader(std::shared_ptr<Shader> shader) { this->mShader = shader; }
	std::shared_ptr<Shader> getShader() const { return mShader; }

	void setName(std::string_view name) { mName = name; }
	const std::string& getName() const { return mName; }

	void setAlbedo(const glm::vec4& color) { mAlbedo = color; }
	glm::vec4 getAlbedo() const { return mAlbedo; }
	void setOpacity(float value) { mOpacity = value; }
	float getOpacity() const { return mOpacity; }

	void setMetallic(float value) { mMetallic = value; }
	float getMetallic() const { return mMetallic; }
	void setRoughness(float value) { mRoughness = value; }
	float getRoughness() const { return mRoughness; }

	void setEmissiveColor(const glm::vec3& color) { mEmissiveColor = color; }
	glm::vec3 getEmissiveColor() const { return mEmissiveColor; }
	void setEmissiveIntensity(float value) { mEmissiveIntensity = value; }
	float getEmissiveIntensity() const { return mEmissiveIntensity; }


	void setReflectivity(float value) { mReflectivity = value; }
	float getReflectivity() const { return mReflectivity; }

	void setAlbedoTexture(std::shared_ptr<Texture> texture) { mAlbedoTexture = texture; }
	std::shared_ptr<Texture> getAlbedoTexture() const { return mAlbedoTexture; }
	void setNormalTexture(std::shared_ptr<Texture> texture) { mNormalTexture = texture; }
	std::shared_ptr<Texture> getNormalTexture() const { return mNormalTexture; }
	void setMetallicTexture(std::shared_ptr<Texture> texture) { mMetallicTexture = texture; }
	std::shared_ptr<Texture> getMetallicTexture() const { return mMetallicTexture; }
	void setRoughnessTexture(std::shared_ptr<Texture> texture) { mRoughnessTexture = texture; }
	std::shared_ptr<Texture> getRoughnessTexture() const { return mRoughnessTexture; }
	void setAOTexture(std::shared_ptr<Texture> texture) { mAOTexture = texture; }
	std::shared_ptr<Texture> getAOTexture() const { return mAOTexture; }
	void setEmissiveTexture(std::shared_ptr<Texture> texture) { mEmissiveTexture = texture; }
	std::shared_ptr<Texture> getEmissiveTexture() const { return mEmissiveTexture; }
	void setMetalRoughTexture(std::shared_ptr<Texture> texture) { mMetalRoughTexture = texture; }
	std::shared_ptr<Texture> getMetalRoughTexture() const { return mMetalRoughTexture; }
	void setOpacityTexture(std::shared_ptr<Texture> texture) { mOpacityTexture = texture; }
	std::shared_ptr<Texture> getOpacityTexture() const { return mOpacityTexture; }

private:
	glm::vec4 mAlbedo = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	float mOpacity = 1.0f;
	float mMetallic = 0.0f;
	float mRoughness = 0.5f;
	glm::vec3 mEmissiveColor = glm::vec3(0.0f);
	float mEmissiveIntensity = 0.0f;
	float mReflectivity = 0.2f;
	std::string mName;

	std::shared_ptr<Texture> mAlbedoTexture = nullptr;
	std::shared_ptr<Texture> mNormalTexture = nullptr;
	std::shared_ptr<Texture> mMetallicTexture = nullptr;
	std::shared_ptr<Texture> mRoughnessTexture = nullptr;
	std::shared_ptr<Texture> mAOTexture = nullptr;
	std::shared_ptr<Texture> mEmissiveTexture = nullptr;
	std::shared_ptr<Texture> mMetalRoughTexture = nullptr;
	std::shared_ptr<Texture> mOpacityTexture = nullptr;
	std::shared_ptr<Shader> mShader = nullptr;
};



