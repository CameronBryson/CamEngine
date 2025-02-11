#include "pch.hpp"
#include "OpenGLFrameBuffer.hpp"
#include <iostream>

OpenGLFrameBuffer::OpenGLFrameBuffer(int width, int height)
    : mWidth(width), mHeight(height)
{
    glGenFramebuffers(1, &mRendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);

    // Create a color texture for color attachment
    glGenTextures(1, &mColorAttachment);
    glBindTexture(GL_TEXTURE_2D, mColorAttachment);
    // For example, an HDR color buffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, mColorAttachment, 0);

    // Create a renderbuffer object for depth/stencil
    glGenRenderbuffers(1, &mRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, mRBO);

    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "OpenGLFrameBuffer: Framebuffer not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteRenderbuffers(1, &mRBO);
    glDeleteTextures(1, &mColorAttachment);
    glDeleteFramebuffers(1, &mRendererID);
}

void OpenGLFrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
}

void OpenGLFrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    // Rebind
    glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);

    // Re-allocate color texture
    glBindTexture(GL_TEXTURE_2D, mColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0,
        GL_RGBA, GL_FLOAT, nullptr);

    // Re-allocate renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
