#include "pch.hpp"
#include "OpenGLFrameBuffer.hpp"
#include <iostream>
#include <cassert>
#include <glad/glad.h>
#include "Texture2D.hpp"
#include <TextureCubemap.hpp>
#include <OpenGLUtil.hpp>

static GLenum toGLInternalFormat(FrameBufferTextureFormat format)
{
    switch (format)
    {
	case FrameBufferTextureFormat::R16F:            return GL_R16F;
    case FrameBufferTextureFormat::RG16F:           return GL_RG16F;
	case FrameBufferTextureFormat::R32F:            return GL_R32F;
    case FrameBufferTextureFormat::RGBA8:           return GL_RGBA8;
    case FrameBufferTextureFormat::RGBA16F:         return GL_RGBA16F;
    case FrameBufferTextureFormat::RGB10A2:         return GL_RGB10_A2;

    case FrameBufferTextureFormat::Depth24:         return GL_DEPTH_COMPONENT24;
    case FrameBufferTextureFormat::Depth32F:        return GL_DEPTH_COMPONENT32F;
    case FrameBufferTextureFormat::Stencil8:        return GL_STENCIL_INDEX8;
    case FrameBufferTextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
    case FrameBufferTextureFormat::Depth32FStencil8:return GL_DEPTH32F_STENCIL8;

    case FrameBufferTextureFormat::None:
    default:
        return 0;
    }
}

static GLenum toGLFormat(FrameBufferTextureFormat format)
{
    switch (format)
    {
	case FrameBufferTextureFormat::R16F:
		return GL_RED;
	case FrameBufferTextureFormat::R32F:
		return GL_RED;
    case FrameBufferTextureFormat::RG16F:
        return GL_RG;
    case FrameBufferTextureFormat::RGBA8:
    case FrameBufferTextureFormat::RGBA16F:
    case FrameBufferTextureFormat::RGB10A2:
        return GL_RGBA;

    case FrameBufferTextureFormat::Depth24:
    case FrameBufferTextureFormat::Depth32F:
        return GL_DEPTH_COMPONENT;

    case FrameBufferTextureFormat::Stencil8:
        return GL_STENCIL_INDEX;

    case FrameBufferTextureFormat::Depth24Stencil8:
    case FrameBufferTextureFormat::Depth32FStencil8:
        return GL_DEPTH_STENCIL;

    case FrameBufferTextureFormat::None:
    default:
        return 0;
    }
}

static GLenum toGLType(FrameBufferTextureFormat format)
{
    switch (format)
    {
	case FrameBufferTextureFormat::R16F:
	case FrameBufferTextureFormat::R32F:
    case FrameBufferTextureFormat::RG16F:
    case FrameBufferTextureFormat::RGBA16F:
    case FrameBufferTextureFormat::Depth32F:
    case FrameBufferTextureFormat::Depth32FStencil8:
        return GL_FLOAT;

    case FrameBufferTextureFormat::RGB10A2:
        return GL_UNSIGNED_INT_2_10_10_10_REV;

    case FrameBufferTextureFormat::Depth24:
    case FrameBufferTextureFormat::Depth24Stencil8:
        return GL_UNSIGNED_INT;

    case FrameBufferTextureFormat::Stencil8:
        return GL_UNSIGNED_BYTE;

    case FrameBufferTextureFormat::RGBA8:
        return GL_UNSIGNED_BYTE;

    case FrameBufferTextureFormat::None:
    default:
        return 0;
    }
}

OpenGLFrameBuffer::OpenGLFrameBuffer(
    int width,
    int height,
    const std::vector<FrameBufferAttachmentSpecification>& attachments,
    int samples
)
    : m_Width(width)
    , m_Height(height)
    , m_Samples(samples)
    , m_AttachmentSpecs(attachments)
{
    m_ViewportX = 0;
    m_ViewportY = 0;
    m_ViewportW = width;
    m_ViewportH = height;
    if (width <= 0 || height <= 0)
        throw std::runtime_error("Invalid framebuffer dimensions");
    if (samples < 1)
        throw std::runtime_error("Invalid sample count");
    createFramebuffer();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    if (m_RendererID != 0)
    {
        GL_CHECK(glDeleteFramebuffers(1, &m_RendererID));
        m_RendererID = 0;
    }
}

void OpenGLFrameBuffer::bind()
{
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
    GL_CHECK(glViewport(m_ViewportX, m_ViewportY, m_ViewportW, m_ViewportH));
}

void OpenGLFrameBuffer::unbind()
{
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void OpenGLFrameBuffer::bindRead()
{
    GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID));
}

void OpenGLFrameBuffer::bindDraw()
{
    GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID));
}

void OpenGLFrameBuffer::resize(int width, int height)
{
    if (width == 0 || height == 0)
    {
        std::cerr << "Warning: Attempted to resize FBO to zero dimension!\n";
        return;
    }

    m_Width = width;
    m_Height = height;

    m_ViewportX = 0;
    m_ViewportY = 0;
    m_ViewportW = width;
    m_ViewportH = height;

    invalidate();
}

void OpenGLFrameBuffer::setViewport(int x, int y, int width, int height)
{
    m_ViewportX = x;
    m_ViewportY = y;
    m_ViewportW = width;
    m_ViewportH = height;
}

void OpenGLFrameBuffer::getViewport(int& x, int& y, int& width, int& height) const
{
    x = m_ViewportX;
    y = m_ViewportY;
    width = m_ViewportW;
    height = m_ViewportH;
}

void OpenGLFrameBuffer::clear(unsigned int mask)
{
    bind();
    GL_CHECK(glClear(mask));
}

void OpenGLFrameBuffer::readPixels(int x, int y, int width, int height, void* data)
{
    bindRead();
    GL_CHECK(glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data));
}

void OpenGLFrameBuffer::addAttachment(const FrameBufferAttachmentSpecification& spec)
{
    m_AttachmentSpecs.push_back(spec);
    invalidate();
}

void OpenGLFrameBuffer::removeAttachment(FrameBufferAttachmentType type, int)
{
    for (auto it = m_AttachmentSpecs.begin(); it != m_AttachmentSpecs.end(); ++it)
    {
        if (it->Type == type)
        {
            m_AttachmentSpecs.erase(it);
            invalidate();
            break;
        }
    }
}

void OpenGLFrameBuffer::invalidate()
{
    if (m_RendererID != 0)
    {
        GL_CHECK(glDeleteFramebuffers(1, &m_RendererID));
        m_RendererID = 0;
    }

    m_ColorAttachments.clear();
    m_DepthAttachment.reset();

    createFramebuffer();
}

void OpenGLFrameBuffer::setSamples(int samples)
{
    m_Samples = samples;
    invalidate();
}

bool OpenGLFrameBuffer::isComplete() const
{
    GLint prevFBO;
    GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO));

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, prevFBO));
    return complete;
}


std::shared_ptr<Texture> OpenGLFrameBuffer::getColorAttachment(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_ColorAttachments.size()))
    {
        return nullptr;
    }
    return m_ColorAttachments[index];
}

std::shared_ptr<Texture> OpenGLFrameBuffer::getDepthAttachment() const
{
    return m_DepthAttachment;
}

void OpenGLFrameBuffer::setDrawBuffers(const std::vector<unsigned int>& attachments)
{
    bindDraw();
    GL_CHECK(glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data()));
}

void OpenGLFrameBuffer::setReadBuffer(unsigned int attachment)
{
    bindRead();
    GL_CHECK(glReadBuffer(attachment));
}

void OpenGLFrameBuffer::blitTo(
    std::shared_ptr<FrameBuffer> dst,
    int srcX0, int srcY0, int srcX1, int srcY1,
    int dstX0, int dstY0, int dstX1, int dstY1,
    unsigned int mask,
    unsigned int filter)
{
    auto glDst = std::dynamic_pointer_cast<OpenGLFrameBuffer>(dst);
    if (!glDst)
    {
        std::cerr << "[OpenGLFrameBuffer::blitTo] Destination is not an OpenGLFrameBuffer!\n";
        return;
    }

    GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID));
    GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glDst->m_RendererID));

    GL_CHECK(glBlitFramebuffer(
        srcX0, srcY0, srcX1, srcY1,
        dstX0, dstY0, dstX1, dstY1,
        mask,
        filter
    ));

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void OpenGLFrameBuffer::attachExternalTexture(
    unsigned int attachment,
    unsigned int target,
    unsigned int textureID,
    int mipLevel)
{
    bind();
    if (target == GL_TEXTURE_CUBE_MAP)
    {
        GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureID, mipLevel));
    }
    else
    {
        GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, textureID, mipLevel));
    }
}


void OpenGLFrameBuffer::createFramebuffer()
{
    GL_CHECK(glGenFramebuffers(1, &m_RendererID));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

    GLuint colorIndex = 0;

    for (const auto& spec : m_AttachmentSpecs)
    {
        GLenum glInternalFormat = toGLInternalFormat(spec.Format);
        GLenum glFormat = toGLFormat(spec.Format);
        GLenum glType = toGLType(spec.Format);

        if (glInternalFormat == 0 || glFormat == 0 || glType == 0)
        {
            std::cerr << "[OpenGLFrameBuffer] Unsupported format or None: "
                << static_cast<int>(spec.Format) << std::endl;
            continue;
        }

        // Create a texture ID
        GLuint texID = 0;
        GL_CHECK(glGenTextures(1, &texID));

        // Decide how to bind texture based on type/samples
        GLenum bindTarget = (m_Samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        // ---------------------------------------------------------
        // Handle the special case: DepthCubemap (Point Shadows)
        // ---------------------------------------------------------
        if (spec.Type == FrameBufferAttachmentType::DepthCubemap && m_Samples <= 1)
        {
            // We only support non-multisampled cubemaps in this example:
            bindTarget = GL_TEXTURE_CUBE_MAP;
            GL_CHECK(glBindTexture(bindTarget, texID));

            // Allocate each of the six faces
            for (int face = 0; face < 6; ++face)
            {
                GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                    0,
                    glInternalFormat,
                    m_Width,
                    m_Height,
                    0,
                    glFormat,
                    glType,
                    nullptr));
            }

            // Setup basic cubemap parameters
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
            // Enable depth comparison for shadow sampling
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
            GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
            // Attach as depth-only
            GL_CHECK(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texID, 0));

            // Wrap the cubemap texture in a TextureCubemap
            auto attachmentTexture = TextureCubemap::createTextureCubemap(texID, m_Width, m_Height);
            m_DepthAttachment = attachmentTexture;

            // Disable color buffer for a pure depth pass
            GL_CHECK(glDrawBuffer(GL_NONE));
            GL_CHECK(glReadBuffer(GL_NONE));

            continue; // Done handling this attachment
        }

        // ---------------------------------------------------------
        // Fallback: 2D texture or 2D multisampled
        // ---------------------------------------------------------
        GL_CHECK(glBindTexture(bindTarget, texID));

        if (m_Samples > 1)
        {
            GL_CHECK(glTexImage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE,
                m_Samples,
                glInternalFormat,
                m_Width,
                m_Height,
                GL_FALSE
            ));
        }
        else
        {
            GL_CHECK(glTexImage2D(
                GL_TEXTURE_2D,
                0,
                glInternalFormat,
                m_Width,
                m_Height,
                0,
                glFormat,
                glType,
                nullptr
            ));

            GL_CHECK(glTexParameteri(bindTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GL_CHECK(glTexParameteri(bindTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL_CHECK(glTexParameteri(bindTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CHECK(glTexParameteri(bindTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        }

        // Wrap as a Texture2D by default
        auto attachmentTexture = Texture2D::createTexture2D(texID, m_Width, m_Height);

        if (spec.Type == FrameBufferAttachmentType::Color)
        {
            GL_CHECK(glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + colorIndex,
                bindTarget,
                texID,
                0
            ));
            m_ColorAttachments.push_back(attachmentTexture);
            colorIndex++;
        }
        else if (spec.Type == FrameBufferAttachmentType::Depth)
        {
            GL_CHECK(glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                bindTarget,
                texID,
                0
            ));
            m_DepthAttachment = attachmentTexture;
        }
        else if (spec.Type == FrameBufferAttachmentType::Stencil)
        {
            GL_CHECK(glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_STENCIL_ATTACHMENT,
                bindTarget,
                texID,
                0
            ));
        }
        else if (spec.Type == FrameBufferAttachmentType::DepthStencil)
        {
            GL_CHECK(glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_STENCIL_ATTACHMENT,
                bindTarget,
                texID,
                0
            ));
            m_DepthAttachment = attachmentTexture;
        }
    }

    // If we have color attachments, set them all; otherwise disable drawing & reading
    if (!m_ColorAttachments.empty())
    {
        std::vector<GLenum> drawBuffers;
        for (GLuint i = 0; i < static_cast<GLuint>(m_ColorAttachments.size()); i++)
        {
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        GL_CHECK(glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data()));
    }
    else
    {
        GL_CHECK(glDrawBuffer(GL_NONE));
        GL_CHECK(glReadBuffer(GL_NONE));
    }

    // Validate completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "[OpenGLFrameBuffer] Framebuffer incomplete! Status = 0x"
            << std::hex << status << std::dec << std::endl;
    }

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


