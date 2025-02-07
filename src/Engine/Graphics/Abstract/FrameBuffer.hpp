#pragma once
#include <memory>
class Texture2D;
class FrameBuffer
{
public:
    virtual ~FrameBuffer() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;
	virtual void resize(int width, int height) = 0;

    static std::shared_ptr<FrameBuffer> createFrameBuffer(int width, int height);
};
