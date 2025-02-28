#pragma once
#include "Engine/Components.hpp"
#include "glm/fwd.hpp"
#include <EnvironmentMap.hpp>
#include <platform.hpp>
#include <UniformBuffer.hpp>
#include "UniformStructs.hpp"
#include <FrameBuffer.hpp>
#include <Texture2D.hpp>
#include "Mesh.hpp"
template<typename T>
class SparseSet;
class BaseScene;
class Shader;
struct RenderItem {
    std::shared_ptr<Mesh> mesh;
    glm::mat4 transform;
    // Distance from camera (used to sort transparent objects)
    float distance;
};
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
    void buildRenderLists();
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
    void taaPass();
	void motionBlurPass();
	void fxaaPass();
    // Resource binding
    void bindSkyboxResources(std::shared_ptr<Shader>& shader);
    void bindShadowMaps(std::shared_ptr<Shader>& shader);

    // Drawing helpers
	void drawRenderList(const std::vector<RenderItem>& renderList, std::shared_ptr<Shader>& shader, bool bindMaterial = false) const;
    void drawImGui();

    void generateSSAOKernel();
	void generateSSAONoise();
    void generateHaltonSequence();

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
	std::shared_ptr<FrameBuffer> mBloomFrameBuffer;
    std::shared_ptr<FrameBuffer> mPingPongFBO[2];
	std::shared_ptr<FrameBuffer> mFXAAFrameBuffer;
	std::shared_ptr<FrameBuffer> mMotionBlurFrameBuffer;
	std::shared_ptr<FrameBuffer> mTAACurrentFrameBuffer;
	std::shared_ptr<FrameBuffer> mTAAPreviousFrameBuffer;


    // Shadow mapping settings
    const int mShadowMapWidth = 512;
    const int mShadowMapHeight = 512;
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
    int bloomBlurPasses = 6;
	bool mFXAAEnabled = true;
    float mFXAAEdgeThreshholdMin = 0.0625f;
	float mFXAAEdgeThreshholdMax = 0.125f;
	float mFXAASubPixelQuality = 0.75f;
	bool mMotionBlurEnabled = true;
	float mMotionBlurStrength = 1.0f;
	int mMotionBlurSamples = 8;
	bool mTAAEnabled = true;
    bool mShowEdges = false;
	float mTAABlendFactor = 0.5f;
	float mEdgeBlendThreshold = 0.1f;
    float mMinBlendAtEdges = 0.3;
    float mEdgeResponsive = 3.0f;
    static constexpr int HALTON_SAMPLES = 16;
    std::vector<glm::vec2> mHaltonPattern;
    int mJitterIndex = 0;
    glm::vec2 mCurrentJitter{0.0f};
    glm::vec2 mPreviousJitter{0.0f};
    float mJitterScale = 0.4f;
    bool mFirstFrame = true;
    

    // SSAO data
    std::shared_ptr<FrameBuffer> mSSAOBuffer;
    std::shared_ptr<FrameBuffer> mSSAOBlurBuffer;
    std::vector<glm::vec3> mSSAOKernel;
    std::shared_ptr<Texture2D> mSSAONoise;

    // SSAO settings
    static const int SSAO_KERNEL_SIZE = 16;
    static const int SSAO_NOISE_SIZE = 4;
    float mSSAORadius = 0.1f;
    float mSSAOBias = 0.05f;
    float mSSAOPower = 0.5f;
    float mSSAOMinDistance = 0.01f;
    float mSSAOMaxDistance = 0.5f;

	float mSSAOBlurRadius = 2.0f;
	float mSSAOBlurDepthThreshold = 0.1f;
	float mSSAOBlurNormalThreshold = 0.1f;


    //Frustum culling
	int mCulledMeshes = 0;
    std::vector<RenderItem> mOpaqueRenderList;
    std::vector<RenderItem> mTransparentRenderList;
};