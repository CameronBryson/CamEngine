#pragma once

#include "FrameBuffer.hpp"
#include "Texture2D.hpp" // If you have a Texture2D wrapper class

#include <memory>
#include <vector>
#include "platform.hpp"

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    // ------------------------------------------------------------------------
    // Constructor / Destructor
    // ------------------------------------------------------------------------
    OpenGLFrameBuffer(int width,
        int height,
        const std::vector<FrameBufferAttachmentSpecification>& attachments,
        int samples = 1);
    virtual ~OpenGLFrameBuffer();

    // ------------------------------------------------------------------------
    // FrameBuffer interface overrides
    // ------------------------------------------------------------------------

    // Binding
    void bind() override;
    void unbind() override;
    void bindRead() override;
    void bindDraw() override;

    // Size & Viewport
    void resize(int width, int height) override;
    int getWidth() const override { return m_Width; }
    int getHeight() const override { return m_Height; }
    void setViewport(int x, int y, int width, int height) override;
    void getViewport(int& x, int& y, int& width, int& height) const override;

    // Clearing & Reading
    void clear(unsigned int mask) override;
    void readPixels(int x, int y, int width, int height, void* data) override;

    // Attachments
    void addAttachment(const FrameBufferAttachmentSpecification& attachmentSpec) override;
    void removeAttachment(FrameBufferAttachmentType type, int index = 0) override;
    const std::vector<FrameBufferAttachmentSpecification>& getAttachments() const override
    {
        return m_AttachmentSpecs;
    }
    void invalidate() override;

    // Multi-sampling
    void setSamples(int samples) override;
    int getSamples() const override { return m_Samples; }

    // Completeness
    bool isComplete() const override;

    // Getting attachments (as Texture2D)
    std::shared_ptr<Texture2D> getColorAttachment(int index = 0) const override;
    std::shared_ptr<Texture2D> getDepthAttachment() const override;

    // Draw / Read buffers
    void setDrawBuffers(const std::vector<unsigned int>& attachments) override;
    void setReadBuffer(unsigned int attachment) override;

    // Blitting
    void blitTo(std::shared_ptr<FrameBuffer> dst,
        int srcX0, int srcY0, int srcX1, int srcY1,
        int dstX0, int dstY0, int dstX1, int dstY1,
        unsigned int mask,
        unsigned int filter) override;

    // Native (OpenGL) handle
    unsigned int getRendererID() const override { return m_RendererID; }

    // Attach external texture
    void attachExternalTexture(unsigned int attachment,
        unsigned int target,
        unsigned int textureID,
        int mipLevel = 0) override;

private:
    // Re-initializes the framebuffer with current specs
    void createFramebuffer();

private:
    // Framebuffer state
    unsigned int m_RendererID = 0; // The OpenGL FBO ID
    int m_Width, m_Height;
    int m_Samples;

    // Stored viewport
    int m_ViewportX = 0;
    int m_ViewportY = 0;
    int m_ViewportW = 0;
    int m_ViewportH = 0;

    // List of attachment specifications (provided externally).
    // We recreate these if the framebuffer is invalidated.
    std::vector<FrameBufferAttachmentSpecification> m_AttachmentSpecs;

    // Storing references to actual textures (color / depth)
    // You could store them in arrays or vectors depending on your needs.
    std::vector<std::shared_ptr<Texture2D>> m_ColorAttachments;
    std::shared_ptr<Texture2D> m_DepthAttachment; // if you want a single depth
};
