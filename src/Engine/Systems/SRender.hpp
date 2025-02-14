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
struct SceneBounds
{
    glm::vec3 center;
    float radius;
    glm::vec3 min;
	glm::vec3 max;
};
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
	void drawModelsShader(std::shared_ptr<Shader>& shader) const;
    void drawImGui();

private:
    BaseScene* mScene;
	SceneBounds mSceneBounds;
    std::shared_ptr<UniformBuffer> mCameraUBO;
	std::shared_ptr<UniformBuffer> mLightUBO;

    std::shared_ptr<FrameBuffer> mShadowMapBuffer;
    bool mShowShadowMap = true;
    const int mShadowMapWidth = 1024;
    const int mShadowMapHeight = 1024;
    void calculateSceneBounds();
    GLuint mDebugQuadVAO = 0;
    GLuint mDebugQuadVBO = 0;
};
