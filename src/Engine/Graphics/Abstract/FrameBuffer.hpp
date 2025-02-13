#pragma once

#include <memory>
#include <vector>

class Texture2D;

enum class FrameBufferAttachmentType
{
    Color,
    Depth,
    Stencil,
    DepthStencil
};

enum class FrameBufferTextureFormat
{
    None = 0,

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
    virtual ~FrameBuffer() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual void bindRead() = 0;
    virtual void bindDraw() = 0;

    virtual void resize(int width, int height) = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;
    virtual void getViewport(int& x, int& y, int& width, int& height) const = 0;

    virtual void clear(unsigned int mask) = 0;

    virtual void readPixels(int x, int y, int width, int height, void* data) = 0;

    virtual void addAttachment(const FrameBufferAttachmentSpecification& attachmentSpec) = 0;

    virtual void removeAttachment(FrameBufferAttachmentType type, int index = 0) = 0;

    virtual const std::vector<FrameBufferAttachmentSpecification>& getAttachments() const = 0;

    virtual void invalidate() = 0;

    virtual void setSamples(int samples) = 0;
    virtual int getSamples() const = 0;

    virtual bool isComplete() const = 0;

    virtual std::shared_ptr<Texture2D> getColorAttachment(int index = 0) const = 0;
    virtual std::shared_ptr<Texture2D> getDepthAttachment() const = 0;

    virtual void setDrawBuffers(const std::vector<unsigned int>& attachments) = 0;

    virtual void setReadBuffer(unsigned int attachment) = 0;

    virtual void blitTo(std::shared_ptr<FrameBuffer> dst,
        int srcX0, int srcY0, int srcX1, int srcY1,
        int dstX0, int dstY0, int dstX1, int dstY1,
        unsigned int mask,
        unsigned int filter) = 0;

    virtual unsigned int getRendererID() const = 0;

    virtual void attachExternalTexture(
        unsigned int attachment,
        unsigned int target,
        unsigned int textureID,
        int mipLevel = 0) = 0;

    static std::shared_ptr<FrameBuffer> createFrameBuffer(
        int width,
        int height,
        const std::vector<FrameBufferAttachmentSpecification>& attachments,
        int samples = 1
    );
};
