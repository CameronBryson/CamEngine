#pragma once
#include <memory>

class FrameBuffer
{
public:
    virtual ~FrameBuffer() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;

    static std::shared_ptr<FrameBuffer> createFrameBuffer(int width, int height);
};
