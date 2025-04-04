#pragma once

#include <memory>
#include <vector>

class Texture;

enum class FrameBufferAttachmentType
{
    Color,
    Depth,
    Stencil,
    DepthStencil,
    DepthCubemap
};

enum class FrameBufferTextureFormat
{
    None = 0,
    R16F,
    R32F,
    RG16F,
    RGBA8,
    RGBA16F,
    RGB10A2,

    Depth24,
    Depth32F,

    Stencil8,

    Depth24Stencil8,
    Depth32FStencil8
};

struct FrameBufferAttachmentSpecification
{
    FrameBufferAttachmentType Type;
    FrameBufferTextureFormat Format;

    FrameBufferAttachmentSpecification(
        FrameBufferAttachmentType type = FrameBufferAttachmentType::Color,
        FrameBufferTextureFormat format = FrameBufferTextureFormat::RGBA8)
        : Type(type), Format(format)
    {
    }
};

class FrameBuffer
{
public:
    // ------------------------------------------------------------------------
    // Constructor / Destructor
    // ------------------------------------------------------------------------
    FrameBuffer(int width,
                      int height,
                      const std::vector<FrameBufferAttachmentSpecification>& attachments,
                      int samples = 1);
    ~FrameBuffer();

    // ------------------------------------------------------------------------
    // FrameBuffer interface overrides
    // ------------------------------------------------------------------------

    // Binding
    void bind();
    void unbind();
    void bindRead();
    void bindDraw();

    // Size & Viewport
    void resize(int width, int height);
    int getWidth() const { return m_Width; }
    int getHeight() const{ return m_Height; }
    void setViewport(int x, int y, int width, int height);
    void getViewport(int& x, int& y, int& width, int& height) const;

    // Clearing & Reading
    void clear(unsigned int mask);
    void readPixels(int x, int y, int width, int height, void* data);

    // Attachments
    void addAttachment(const FrameBufferAttachmentSpecification& attachmentSpec);
    void removeAttachment(FrameBufferAttachmentType type, int index = 0);
    const std::vector<FrameBufferAttachmentSpecification>& getAttachments() const
    {
        return m_AttachmentSpecs;
    }
    void invalidate();

    // Multi-sampling
    void setSamples(int samples);
    int getSamples() const{ return m_Samples; }

    // Completeness
    bool isComplete() const;

    // Getting attachments (as Texture2D)
    std::shared_ptr<Texture> getColorAttachment(int index = 0) const;
    std::shared_ptr<Texture> getDepthAttachment() const;

    // Draw / Read buffers
    void setDrawBuffers(const std::vector<unsigned int>& attachments);
    void setReadBuffer(unsigned int attachment);

    // Blitting
    void blitTo(std::shared_ptr<FrameBuffer> dst,
                int srcX0, int srcY0, int srcX1, int srcY1,
                int dstX0, int dstY0, int dstX1, int dstY1,
                unsigned int mask,
                unsigned int filter);

    // Native (OpenGL) handle
    unsigned int getRendererID() const { return m_RendererID; }

    // Attach external texture
    void attachExternalTexture(unsigned int attachment,
                               unsigned int target,
                               unsigned int textureID,
                               int mipLevel = 0);

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
    std::vector<std::shared_ptr<Texture>> m_ColorAttachments;
    std::shared_ptr<Texture> m_DepthAttachment; // if you want a single depth
};
