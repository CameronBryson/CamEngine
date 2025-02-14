#pragma once
#include "Engine/Components.hpp"
#include "glm/fwd.hpp"
#include <EnvironmentMap.hpp>
#include <platform.hpp>
#include <UniformBuffer.hpp>
#include "UniformStructs.hpp"
#include <FrameBuffer.hpp>
template<typename T>
class SparseSet;
class BaseScene;
class Shader;

class SRender
{
public:
    SRender(BaseScene* scene);
    void init();
    void render();
    void shutdown();

    void buildDirectionalLights(LightData& lightData);
    void buildPointLights(LightData& lightData);
	void buildSpotLights(LightData& lightData);
    void drawModels() const;
	void drawModelsShadow(std::shared_ptr<Shader>& shader) const;
	void drawImGui() const;

private:
    BaseScene* mScene;
    std::shared_ptr<UniformBuffer> mCameraUBO;
	std::shared_ptr<UniformBuffer> mLightUBO;

    std::vector<std::shared_ptr<FrameBuffer>> mDirectionalShadowFramebuffers;

    // Shadow map resolution and texture unit base.
    const int mShadowMapWidth = 1024;
    const int mShadowMapHeight = 1024;
    std::shared_ptr<FrameBuffer> mHDRFramebuffer;

};
