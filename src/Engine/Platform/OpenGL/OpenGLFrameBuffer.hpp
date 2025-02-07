#pragma once
#include "Engine/Graphics/Abstract/FrameBuffer.hpp"
#include "Engine/Util/platform.hpp"

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer(int width, int height);
    ~OpenGLFrameBuffer() override;

    void bind() override;
    void unbind() override;

private:
    GLuint mRendererID = 0;
    GLuint mColorAttachment = 0;
    GLuint mRBO = 0;
    int mWidth = 0;
    int mHeight = 0;
};
