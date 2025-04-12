#include "pch.hpp"
#include "FrameBuffer.hpp"
#include <iostream>
#include <cassert>
#include <glad/glad.h>
#include "Texture.hpp"
#include <OpenGLUtil.hpp>
#include "Engine/Util/Logging.hpp"
#include "Engine/Util/ErrorHandler.hpp"

namespace {
    // Helper function to convert FrameBufferTextureFormat to OpenGL internal format
    GLenum toGLInternalFormat(FrameBufferTextureFormat format)
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
            LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer texture format");
            return 0;
        }
    }

    // Helper function to convert FrameBufferTextureFormat to OpenGL format
    GLenum toGLFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
        case FrameBufferTextureFormat::R16F:
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
            LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer texture format");
            return 0;
        }
    }

    // Helper function to convert FrameBufferTextureFormat to OpenGL data type
    GLenum toGLType(FrameBufferTextureFormat format)
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
            LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer texture format");
            return 0;
        }
    }

    // Convert attachment type to OpenGL attachment point
    GLenum getAttachmentPoint(FrameBufferAttachmentType type, int index = 0)
    {
        switch (type)
        {
        case FrameBufferAttachmentType::Color:
            return GL_COLOR_ATTACHMENT0 + index;
        case FrameBufferAttachmentType::Depth:
        case FrameBufferAttachmentType::DepthCubemap:
            return GL_DEPTH_ATTACHMENT;
        case FrameBufferAttachmentType::Stencil:
            return GL_STENCIL_ATTACHMENT;
        case FrameBufferAttachmentType::DepthStencil:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        default:
            LOG_ERROR(logging::gGraphicsLogger, "Invalid framebuffer attachment type");
            return 0;
        }
    }

    // Helper to check framebuffer status and log any issues
    bool checkFramebufferStatus(GLuint /*fbo*/, const std::string& label)
    {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string errorMsg;
            switch (status)
            {
            case GL_FRAMEBUFFER_UNDEFINED:
                errorMsg = "GL_FRAMEBUFFER_UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                errorMsg = "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                errorMsg = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
                break;
            default:
                errorMsg = "Unknown framebuffer error: " + std::to_string(status);
                break;
            }
            LOG_ERROR(logging::gGraphicsLogger, "Framebuffer '{0}' is incomplete: {1}", label, errorMsg);
            return false;
        }
        return true;
    }
}

FrameBuffer::FrameBuffer(
    int width,
    int height,
    std::vector<FrameBufferAttachmentSpecification> attachments,
    int samples,
    std::string label)
    : mWidth(width)
    , mHeight(height)
    , mSamples(samples)
    , mAttachmentSpecs(std::move(attachments))
    , mLabel(std::move(label))
{
    try {
        LOG_DEBUG(logging::gGraphicsLogger, "Creating framebuffer: {0} ({1}x{2}, {3} samples)", 
                  label.empty() ? "unnamed" : label, width, height, samples);
                  
        if (width <= 0 || height <= 0)
            throw error_handling::GraphicsException("Invalid framebuffer dimensions");
            
        if (samples < 1)
            throw error_handling::GraphicsException("Invalid sample count");
            
        mViewportX = 0;
        mViewportY = 0;
        mViewportW = width;
        mViewportH = height;
        
        createFramebuffer();
    }
    catch (const std::exception& e) {
        LOG_ERROR(logging::gGraphicsLogger, "Failed to create framebuffer: {0}", e.what());
        throw;
    }
}

FrameBuffer::~FrameBuffer()
{
    cleanup();
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : mRendererID(other.mRendererID)
    , mWidth(other.mWidth)
    , mHeight(other.mHeight)
    , mSamples(other.mSamples)
    , mLabel(std::move(other.mLabel))
    , mViewportX(other.mViewportX)
    , mViewportY(other.mViewportY)
    , mViewportW(other.mViewportW)
    , mViewportH(other.mViewportH)
    , mAttachmentSpecs(std::move(other.mAttachmentSpecs))
    , mColorAttachments(std::move(other.mColorAttachments))
    , mDepthAttachment(std::move(other.mDepthAttachment))
{
    // Prevent double deletion by marking other's ID as 0
    other.mRendererID = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
    if (this != &other)
    {
        // Clean up existing resources
        cleanup();
        
        // Move resources from other
        mRendererID = other.mRendererID;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mSamples = other.mSamples;
        mLabel = std::move(other.mLabel);
        mViewportX = other.mViewportX;
        mViewportY = other.mViewportY;
        mViewportW = other.mViewportW;
        mViewportH = other.mViewportH;
        mAttachmentSpecs = std::move(other.mAttachmentSpecs);
        mColorAttachments = std::move(other.mColorAttachments);
        mDepthAttachment = std::move(other.mDepthAttachment);
        
        // Prevent double deletion
        other.mRendererID = 0;
    }
    return *this;
}

// Implementation of OpenGL wrapper methods
void FrameBuffer::genFramebuffers(unsigned int count, unsigned int* ids) {
    GL_CHECK(glGenFramebuffers(count, ids));
}

void FrameBuffer::deleteFramebuffers(unsigned int count, const unsigned int* ids) {
    GL_CHECK(glDeleteFramebuffers(count, ids));
}

void FrameBuffer::bindFramebuffer(unsigned int target, unsigned int framebuffer) {
    GL_CHECK(glBindFramebuffer(target, framebuffer));
}

void FrameBuffer::drawBuffer(unsigned int buffer) {
    GL_CHECK(glDrawBuffer(buffer));
}

void FrameBuffer::drawBuffers(unsigned int n, const unsigned int* bufs) {
    GL_CHECK(glDrawBuffers(n, bufs));
}

void FrameBuffer::readBuffer(unsigned int src) {
    GL_CHECK(glReadBuffer(src));
}

void FrameBuffer::blitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1,
                                int dstX0, int dstY0, int dstX1, int dstY1,
                                unsigned int mask, unsigned int filter) {
    GL_CHECK(glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter));
}

void FrameBuffer::clearBuffers(unsigned int mask) {
    GL_CHECK(glClear(mask));
}

void FrameBuffer::viewport(int x, int y, int width, int height) {
    GL_CHECK(glViewport(x, y, width, height));
}

void FrameBuffer::readPixelsBuffer(int x, int y, int width, int height, unsigned int format, 
                                 unsigned int type, void* data) {
    GL_CHECK(glReadPixels(x, y, width, height, format, type, data));
}

unsigned int FrameBuffer::checkFramebufferStatus(unsigned int target) {
    return glCheckFramebufferStatus(target);
}

void FrameBuffer::getIntegerv(unsigned int pname, int* params) {
    GL_CHECK(glGetIntegerv(pname, params));
}

// Update FrameBuffer methods to use the new wrapper methods

void FrameBuffer::cleanup()
{
    if (mRendererID != 0)
    {
        LOG_DEBUG(logging::gGraphicsLogger, "Deleting framebuffer: {0} (ID: {1})",
                  mLabel.empty() ? "unnamed" : mLabel, mRendererID);
                  
        deleteFramebuffers(1, &mRendererID);
        mRendererID = 0;
    }
    
    // We don't need to explicitly delete the textures as they're managed by shared_ptr
}

void FrameBuffer::bind()
{
    bindFramebuffer(GL_FRAMEBUFFER, mRendererID);
    viewport(mViewportX, mViewportY, mViewportW, mViewportH);
    
    LOG_TRACE(logging::gGraphicsLogger, "Bound framebuffer: {0} (ID: {1})",
              mLabel.empty() ? "unnamed" : mLabel, mRendererID);
}

void FrameBuffer::unbind()
{
    bindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindRead()
{
    bindFramebuffer(GL_READ_FRAMEBUFFER, mRendererID);
}

void FrameBuffer::bindDraw()
{
    bindFramebuffer(GL_DRAW_FRAMEBUFFER, mRendererID);
}

void FrameBuffer::resize(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        LOG_WARN(logging::gGraphicsLogger, "Attempted to resize FBO '{0}' to invalid dimensions: {1}x{2}",
                mLabel.empty() ? "unnamed" : mLabel, width, height);
        return;
    }

    if (width == mWidth && height == mHeight)
    {
        LOG_DEBUG(logging::gGraphicsLogger, "Skipping resize of FBO '{0}' - dimensions unchanged: {1}x{2}",
                 mLabel.empty() ? "unnamed" : mLabel, width, height);
        return;
    }

    LOG_DEBUG(logging::gGraphicsLogger, "Resizing FBO '{0}' from {1}x{2} to {3}x{4}",
             mLabel.empty() ? "unnamed" : mLabel, mWidth, mHeight, width, height);

    mWidth = width;
    mHeight = height;

    // Update viewport to match if it was previously matching
    if (mViewportW == mWidth && mViewportH == mHeight)
    {
        mViewportW = width;
        mViewportH = height;
    }

    invalidate();
}

void FrameBuffer::setViewport(int x, int y, int width, int height)
{
    mViewportX = x;
    mViewportY = y;
    mViewportW = width;
    mViewportH = height;
}

void FrameBuffer::getViewport(int& x, int& y, int& width, int& height) const
{
    x = mViewportX;
    y = mViewportY;
    width = mViewportW;
    height = mViewportH;
}

void FrameBuffer::clear(unsigned int mask)
{
    bind();
    clearBuffers(mask);
}

void FrameBuffer::readPixels(int x, int y, int width, int height, void* data, unsigned int format, unsigned int type)
{
    if (!data)
    {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot read pixels to null data pointer");
        return;
    }

    // Validate coordinates are within bounds
    if (x < 0 || y < 0 || x + width > mWidth || y + height > mHeight)
    {
        LOG_WARN(logging::gGraphicsLogger, 
            "ReadPixels region ({0},{1},{2},{3}) exceeds framebuffer dimensions ({4}x{5})",
            x, y, width, height, mWidth, mHeight);
    }

    bindRead();
    readPixelsBuffer(x, y, width, height, format, type, data);
}

int FrameBuffer::addAttachment(FrameBufferAttachmentSpecification attachmentSpec)
{
    LOG_DEBUG(logging::gGraphicsLogger, "Adding attachment to framebuffer '{0}'",
             mLabel.empty() ? "unnamed" : mLabel);
             
    int index = 0;
    if (attachmentSpec.Type == FrameBufferAttachmentType::Color)
    {
        // Count existing color attachments for index
        for (const auto& spec : mAttachmentSpecs)
        {
            if (spec.Type == FrameBufferAttachmentType::Color)
                index++;
        }
    }
    
    mAttachmentSpecs.push_back(std::move(attachmentSpec));
    invalidate();
    
    return attachmentSpec.Type == FrameBufferAttachmentType::Color ? index : 0;
}

bool FrameBuffer::removeAttachment(FrameBufferAttachmentType type, int index)
{
    bool removed = false;
    
    if (type == FrameBufferAttachmentType::Color)
    {
        // Find the specific color attachment at the given index
        int colorIndex = 0;
        for (auto it = mAttachmentSpecs.begin(); it != mAttachmentSpecs.end(); ++it)
        {
            if (it->Type == FrameBufferAttachmentType::Color)
            {
                if (colorIndex == index)
                {
                    LOG_DEBUG(logging::gGraphicsLogger, "Removing color attachment {0} from framebuffer '{1}'",
                             index, mLabel.empty() ? "unnamed" : mLabel);
                    mAttachmentSpecs.erase(it);
                    removed = true;
                    break;
                }
                colorIndex++;
            }
        }
    }
    else
    {
        // For other types, just remove the first match
        for (auto it = mAttachmentSpecs.begin(); it != mAttachmentSpecs.end(); ++it)
        {
            if (it->Type == type)
            {
                LOG_DEBUG(logging::gGraphicsLogger, "Removing attachment of type {0} from framebuffer '{1}'",
                         static_cast<int>(type), mLabel.empty() ? "unnamed" : mLabel);
                mAttachmentSpecs.erase(it);
                removed = true;
                break;
            }
        }
    }
    
    if (removed)
    {
        invalidate();
    }
    else
    {
        LOG_WARN(logging::gGraphicsLogger, "No matching attachment found to remove from framebuffer '{0}'",
               mLabel.empty() ? "unnamed" : mLabel);
    }
    
    return removed;
}

void FrameBuffer::invalidate()
{
    LOG_DEBUG(logging::gGraphicsLogger, "Invalidating framebuffer '{0}' (ID: {1})",
             mLabel.empty() ? "unnamed" : mLabel, mRendererID);
             
    // Clean up existing framebuffer if there is one
    if (mRendererID != 0)
    {
        deleteFramebuffers(1, &mRendererID);
        mRendererID = 0;
    }

    // Clear attachment collections (textures will be freed by shared_ptr)
    mColorAttachments.clear();
    mDepthAttachment.reset();

    // Recreate with updated specifications
    createFramebuffer();
}

void FrameBuffer::setSamples(int samples)
{
    if (samples < 1)
    {
        LOG_WARN(logging::gGraphicsLogger, "Invalid sample count {0}, must be at least 1", samples);
        return;
    }
    
    if (samples == mSamples)
    {
        return; // No change needed
    }
    
    LOG_DEBUG(logging::gGraphicsLogger, "Changing FBO '{0}' samples from {1} to {2}",
             mLabel.empty() ? "unnamed" : mLabel, mSamples, samples);
             
    mSamples = samples;
    invalidate();
}

bool FrameBuffer::isComplete() const
{
    if (mRendererID == 0)
    {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot check completeness of invalid framebuffer");
        return false;
    }

    GLint prevFBO;
    getIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    
    bindFramebuffer(GL_FRAMEBUFFER, mRendererID);
    bool complete = checkFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    
    if (!complete)
    {
        // Use our helper function to get detailed error information
        ::checkFramebufferStatus(mRendererID, mLabel);
    }
    
    bindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    return complete;
}

std::shared_ptr<Texture> FrameBuffer::getColorAttachment(int index) const
{
    if (index < 0 || index >= static_cast<int>(mColorAttachments.size()))
    {
        if (!mColorAttachments.empty()) // Only log if there are attachments
        {
            LOG_WARN(logging::gGraphicsLogger, 
                "Requesting invalid color attachment index {0} (max: {1}) from framebuffer '{2}'", 
                index, mColorAttachments.size() - 1, mLabel.empty() ? "unnamed" : mLabel);
        }
        return nullptr;
    }
    return mColorAttachments[index];
}

void FrameBuffer::setDrawBuffers(const std::vector<unsigned int>& attachments)
{
    bindDraw();
    if (attachments.empty())
    {
        drawBuffer(GL_NONE);
    }
    else
    {
        drawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
    }
}

void FrameBuffer::setReadBuffer(unsigned int attachment)
{
    bindRead();
    readBuffer(attachment);
}

void FrameBuffer::blitTo(
    std::shared_ptr<FrameBuffer> dst,
    int srcX0, int srcY0, int srcX1, int srcY1,
    int dstX0, int dstY0, int dstX1, int dstY1,
    unsigned int mask,
    unsigned int filter)
{
    if (!dst)
    {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot blit to null framebuffer");
        return;
    }

    // Source region validation
    if (srcX0 < 0 || srcY0 < 0 || srcX1 > mWidth || srcY1 > mHeight || 
        srcX0 >= srcX1 || srcY0 >= srcY1)
    {
        LOG_WARN(logging::gGraphicsLogger, "Invalid source region for blit: ({0},{1}) to ({2},{3})",
               srcX0, srcY0, srcX1, srcY1);
    }
    
    // Destination region validation
    if (dstX0 < 0 || dstY0 < 0 || dstX1 > dst->getWidth() || dstY1 > dst->getHeight() ||
        dstX0 >= dstX1 || dstY0 >= dstY1)
    {
        LOG_WARN(logging::gGraphicsLogger, "Invalid destination region for blit: ({0},{1}) to ({2},{3})",
               dstX0, dstY0, dstX1, dstY1);
    }

    bindFramebuffer(GL_READ_FRAMEBUFFER, mRendererID);
    bindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->getRendererID());

    blitFramebuffer(
        srcX0, srcY0, srcX1, srcY1,
        dstX0, dstY0, dstX1, dstY1,
        mask,
        filter
    );

    // Restore default framebuffer
    bindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resolveToFBO(std::shared_ptr<FrameBuffer> dst, unsigned int mask)
{
    if (!dst)
    {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot resolve to null framebuffer");
        return;
    }

    if (mSamples <= 1)
    {
        LOG_WARN(logging::gGraphicsLogger, "Attempting to resolve a non-multisampled framebuffer");
    }

    if (dst->getSamples() > 1)
    {
        LOG_WARN(logging::gGraphicsLogger, "Resolving to a multisampled framebuffer is not recommended");
    }

    // Use full framebuffer dimensions
    blitTo(dst, 
        0, 0, mWidth, mHeight,
        0, 0, dst->getWidth(), dst->getHeight(),
        mask, GL_NEAREST);
}

void FrameBuffer::attachExternalTexture(
    unsigned int attachment,
    unsigned int target,
    unsigned int textureID,
    int mipLevel)
{
    if (textureID == 0)
    {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot attach invalid (zero) texture ID");
        return;
    }

    bind();
    
    if (target == GL_TEXTURE_CUBE_MAP)
    {
        Texture::framebufferTexture(GL_FRAMEBUFFER, attachment, textureID, mipLevel);
    }
    else
    {
        Texture::framebufferTexture2D(GL_FRAMEBUFFER, attachment, target, textureID, mipLevel);
    }
    
    // Verify attachment was successful
    unsigned int status = checkFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR(logging::gGraphicsLogger, 
            "Failed to attach external texture to framebuffer '{0}' (status: 0x{1:x})",
            mLabel.empty() ? "unnamed" : mLabel, status);
    }
}

void FrameBuffer::setLabel(const std::string& label)
{
    mLabel = label;
    
    // Set debug label if supported and ID exists
    if (mRendererID != 0)
    {
        gl::labelObject(GL_FRAMEBUFFER, mRendererID, label.c_str());
    }
}

void FrameBuffer::createFramebuffer()
{
    LOG_DEBUG(logging::gGraphicsLogger, "Creating framebuffer with {0} attachment specifications", 
              mAttachmentSpecs.size());

    genFramebuffers(1, &mRendererID);
    if (mRendererID == 0)
    {
        throw error_handling::GraphicsException("Failed to generate framebuffer ID");
    }
    
    // Set debug label if we have one
    if (!mLabel.empty())
    {
        gl::labelObject(GL_FRAMEBUFFER, mRendererID, mLabel.c_str());
    }

    bindFramebuffer(GL_FRAMEBUFFER, mRendererID);

    GLuint colorAttachmentIndex = 0;
    std::vector<GLenum> drawBuffers;

    for (const auto& spec : mAttachmentSpecs)
    {
        GLenum glInternalFormat = toGLInternalFormat(spec.Format);
        GLenum glFormat = toGLFormat(spec.Format);
        GLenum glType = toGLType(spec.Format);

        if (glInternalFormat == 0 || glFormat == 0 || glType == 0)
        {
            LOG_ERROR(logging::gGraphicsLogger, "Unsupported framebuffer format: {0}",
                     static_cast<int>(spec.Format));
            continue;
        }

        // Create a texture ID
        GLuint texID = 0;
        Texture::genTextures(1, &texID);
        if (texID == 0)
        {
            LOG_ERROR(logging::gGraphicsLogger, "Failed to generate texture ID for framebuffer attachment");
            continue;
        }

        // Set debug label for texture if we have attachment name
        if (!spec.Name.empty())
        {
            std::string texLabel = spec.Name + " Texture";
            gl::labelObject(GL_TEXTURE, texID, texLabel.c_str());
        }
        else if (!mLabel.empty())
        {
            std::string texLabel = mLabel;
            switch (spec.Type)
            {
            case FrameBufferAttachmentType::Color:
                texLabel += " Color" + std::to_string(colorAttachmentIndex);
                break;
            case FrameBufferAttachmentType::Depth:
                texLabel += " Depth";
                break;
            case FrameBufferAttachmentType::Stencil:
                texLabel += " Stencil";
                break;
            case FrameBufferAttachmentType::DepthStencil:
                texLabel += " DepthStencil";
                break;
            case FrameBufferAttachmentType::DepthCubemap:
                texLabel += " DepthCube";
                break;
            }
            gl::labelObject(GL_TEXTURE, texID, texLabel.c_str());
        }

        // Decide how to bind texture based on type/samples
        GLenum bindTarget = (mSamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

        // Special handling for cubemap depth attachment
        if (spec.Type == FrameBufferAttachmentType::DepthCubemap && mSamples <= 1)
        {
            bindTarget = GL_TEXTURE_CUBE_MAP;
            Texture::bindTexture(bindTarget, texID);

            for (int face = 0; face < 6; ++face)
            {
                Texture::texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                  0,
                                  glInternalFormat,
                                  mWidth,
                                  mHeight,
                                  0,
                                  glFormat,
                                  glType,
                                  nullptr);
            }

            // Setup cubemap parameters
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            // Enable depth comparison for shadow sampling
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            Texture::texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            
            // Attach as depth-only
            Texture::framebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texID, 0);

            // Wrap in our universal Texture class
            auto attachmentTexture = std::make_shared<Texture>(texID, mWidth, mHeight, Texture::Type::CUBEMAP);
            
            mDepthAttachment = attachmentTexture;

            // Disable color buffer for a depth-only cubemap
            drawBuffer(GL_NONE);
            readBuffer(GL_NONE);

            continue; // Done handling this attachment
        }

        // Regular 2D texture or 2D multisampled texture handling
        Texture::bindTexture(bindTarget, texID);

        if (mSamples > 1)
        {
            // Multisampled texture
            Texture::texImage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE,
                mSamples,
                glInternalFormat,
                mWidth,
                mHeight,
                GL_FALSE
            );
        }
        else
        {
            // Regular 2D texture
            Texture::texImage2D(
                GL_TEXTURE_2D,
                0,
                glInternalFormat,
                mWidth,
                mHeight,
                0,
                glFormat,
                glType,
                nullptr
            );

            // Set texture parameters for non-multisampled textures
            Texture::texParameteri(bindTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            Texture::texParameteri(bindTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            Texture::texParameteri(bindTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            Texture::texParameteri(bindTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        // Create and attach appropriate texture type based on attachment spec
        std::shared_ptr<Texture> attachmentTexture;
        
        switch (spec.Type)
        {
        case FrameBufferAttachmentType::Color:
            {
                attachmentTexture = std::make_shared<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);
                GLenum attachmentPoint = GL_COLOR_ATTACHMENT0 + colorAttachmentIndex;
                
                if (mSamples > 1)
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        attachmentPoint,
                        GL_TEXTURE_2D_MULTISAMPLE, 
                        texID, 
                        0
                    );
                }
                else
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        attachmentPoint,
                        GL_TEXTURE_2D, 
                        texID, 
                        0
                    );
                }
                
                mColorAttachments.push_back(attachmentTexture);
                drawBuffers.push_back(attachmentPoint);
                colorAttachmentIndex++;
                break;
            }
            
        case FrameBufferAttachmentType::Depth:
            {
                attachmentTexture = std::make_shared<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);
                
                if (mSamples > 1)
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        GL_DEPTH_ATTACHMENT,
                        GL_TEXTURE_2D_MULTISAMPLE, 
                        texID, 
                        0
                    );
                }
                else
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        GL_DEPTH_ATTACHMENT,
                        GL_TEXTURE_2D, 
                        texID, 
                        0
                    );
                }
                
                mDepthAttachment = attachmentTexture;
                break;
            }
            
        case FrameBufferAttachmentType::Stencil:
            {
                attachmentTexture = std::make_shared<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);
                
                if (mSamples > 1)
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        GL_STENCIL_ATTACHMENT,
                        GL_TEXTURE_2D_MULTISAMPLE, 
                        texID, 
                        0
                    );
                }
                else
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        GL_STENCIL_ATTACHMENT,
                        GL_TEXTURE_2D, 
                        texID, 
                        0
                    );
                }
                break;
            }
            
        case FrameBufferAttachmentType::DepthStencil:
            {
                attachmentTexture = std::make_shared<Texture>(texID, mWidth, mHeight, Texture::Type::TEXTURE_2D);
                
                if (mSamples > 1)
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        GL_DEPTH_STENCIL_ATTACHMENT,
                        GL_TEXTURE_2D_MULTISAMPLE, 
                        texID, 
                        0
                    );
                }
                else
                {
                    Texture::framebufferTexture2D(
                        GL_FRAMEBUFFER, 
                        GL_DEPTH_STENCIL_ATTACHMENT,
                        GL_TEXTURE_2D, 
                        texID, 
                        0
                    );
                }
                
                mDepthAttachment = attachmentTexture;
                break;
            }
            
        default:
            LOG_WARN(logging::gGraphicsLogger, "Unhandled attachment type: {0}", 
                   static_cast<int>(spec.Type));
        }
    }

    // Configure draw/read buffers based on attachments
    if (!drawBuffers.empty())
    {
        this->drawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    }
    else if (mDepthAttachment)
    {
        // Depth-only framebuffer
        drawBuffer(GL_NONE);
        readBuffer(GL_NONE);
    }
    else
    {
        LOG_WARN(logging::gGraphicsLogger, "Creating framebuffer with no attachments");
    }

    // Validate completeness and get any error information
    bool complete = ::checkFramebufferStatus(mRendererID, mLabel);
    
    if (!complete)
    {
        throw error_handling::GraphicsException(
            "Framebuffer " + (mLabel.empty() ? "unnamed" : mLabel) + " is incomplete");
    }

    bindFramebuffer(GL_FRAMEBUFFER, 0);
}


