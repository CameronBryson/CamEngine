#pragma once
#include <memory>

class UniformBuffer
{
public:
    virtual ~UniformBuffer() = default;

    // Update buffer contents
    virtual void setData(const void* data, unsigned int size, unsigned int offset = 0) = 0;

    static std::shared_ptr<UniformBuffer> createUniformBuffer(unsigned int size, unsigned int binding);
};
