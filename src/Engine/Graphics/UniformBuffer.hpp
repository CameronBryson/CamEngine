#pragma once
#include "Engine/Util/platform.hpp"

class UniformBuffer
{
public:
    UniformBuffer(unsigned int size, unsigned int binding);
    ~UniformBuffer();
    void setData(const void* data, unsigned int size, unsigned int offset = 0);
    unsigned int getID(){ return mRendererID; }

private:
    GLuint mRendererID;
    unsigned int mSize;
    unsigned int mBinding;
};
