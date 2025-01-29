#pragma once
#include "Engine/Components.hpp"
#include "glm/fwd.hpp"
#include <platform.hpp>
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

    void buildDirectionalLights(const Shader& shader);
	void buildPointLights(const Shader& shader);
    void drawModels(const Shader& shader) const;
	void drawImGui() const;

private:
    BaseScene* mScene;

    // HDR Framebuffer
    GLuint hdrFBO;
    GLuint hdrColorBuffer;

    // MSAA Framebuffer
    GLuint msFBO;
    GLuint msColorBuffer;
    GLuint msDepthBuffer;

    // Framebuffer dimensions
    unsigned int width;
    unsigned int height;

    // Initialize Framebuffers
    void setupHDRFramebuffer();
    void setupMSAAFramebuffer();
};
