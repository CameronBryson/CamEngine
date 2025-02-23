#pragma once
#include "Engine/Components.hpp"
#include "glm/fwd.hpp"
#include <EnvironmentMap.hpp>
#include <platform.hpp>
#include <UniformBuffer.hpp>
#include "UniformStructs.hpp"
#include <FrameBuffer.hpp>
#include <Texture2D.hpp>
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
class SRender {
public:
    explicit SRender(BaseScene* scene);

    // Core system functions
    void init();
    void lateInit();
    void render();
    void shutdown();

private:
    // Initialization helpers
    void initImGui();
    void initFramebuffers();
    void calculateSceneBounds();

    // Light data building
    void buildLightData(LightData& lightData);
    void buildDirectionalLights(LightData& lightData);
    void buildPointLights(LightData& lightData);
    void buildSpotLights(LightData& lightData);

    // Render passes
    void updateCameraUniforms();
    void shadowPass(const LightData& lightData);
    void depthPass();
    void geometryPass();
    void ssaoPass();
    void lightingPass();
    void postProcessPass();

    // Shadow mapping
    void renderDirectionalShadows(const LightData& lightData);
    void renderSpotShadows(const LightData& lightData);
    void renderPointShadows(const LightData& lightData);

    // Post-processing
    void bloomPass();
    void hdrPass();

    // Resource binding
    void bindSkyboxResources(std::shared_ptr<Shader>& shader);
    void bindShadowMaps(std::shared_ptr<Shader>& shader);

    // Drawing helpers
    void drawModels() const;
    void drawModels(std::shared_ptr<Shader>& shader, bool bindMatieral = false) const;
    void drawImGui();

    void generateSSAOKernel();
	void generateSSAONoise();

    // Scene data
    BaseScene* mScene;
    SceneBounds mSceneBounds;

    // Uniform buffers
    std::shared_ptr<UniformBuffer> mCameraUBO;
    std::shared_ptr<UniformBuffer> mLightUBO;

    // Framebuffers
    std::shared_ptr<FrameBuffer> mGBuffer;
    std::shared_ptr<FrameBuffer> mDirectionalShadowMapBuffer;
    std::shared_ptr<FrameBuffer> mSpotShadowMapBuffer;
    std::shared_ptr<FrameBuffer> mPointShadwMapBuffer;
    std::shared_ptr<FrameBuffer> mHDRFrameBuffer;
    std::shared_ptr<FrameBuffer> mPingPongFBO[2];

    // Shadow mapping settings
    const int mShadowMapWidth = 2048;
    const int mShadowMapHeight = 2048;
    bool mEnableShadows = true;
    float nearPlane = 1.0f;
    float farPlane = 25.0f;

    // Post-processing settings
    float mExposure = 1.0f;
    bool mHDR = true;
    bool bloomEnabled = true;
	bool ssaoEnabled = true;
    float bloomThreshold = 1.0f;
    float bloomStrength = 1.0f;
    int blurPasses = 10;

    // SSAO data
    std::shared_ptr<FrameBuffer> mSSAOBuffer;
    std::shared_ptr<FrameBuffer> mSSAOBlurBuffer;
    std::vector<glm::vec3> mSSAOKernel;
    std::shared_ptr<Texture2D> mSSAONoise;

    // SSAO settings
    static const int SSAO_KERNEL_SIZE = 64;
    static const int SSAO_NOISE_SIZE = 4;
    float mSSAORadius = 0.5f;
    float mSSAOBias = 0.025f;
    float mSSAOPower = 1.0f;
};