#pragma once
#include "Engine/Graphics/Abstract/UniformBuffer.hpp"
#include "Engine/Util/platform.hpp"

class OpenGLUniformBuffer : public UniformBuffer
{
public:
    OpenGLUniformBuffer(unsigned int size, unsigned int binding);
    ~OpenGLUniformBuffer() override;
    void setData(const void* data, unsigned int size, unsigned int offset = 0) override;
	unsigned int getID() override { return mRendererID; }

private:
    GLuint mRendererID;
    unsigned int mSize;
	unsigned int mBinding;
};
