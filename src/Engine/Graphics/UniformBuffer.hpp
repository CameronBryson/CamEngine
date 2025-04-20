#pragma once
#include "Engine/Util/platform.hpp"

class UniformBuffer
{
public:
	UniformBuffer(unsigned int size, unsigned int binding);
	~UniformBuffer();

	// Delete copy operations
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	// Add move operations
	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;

	void setData(const void* data, unsigned int size, unsigned int offset = 0);
	unsigned int getID() const { return mRendererID; }

private:
	GLuint mRendererID = 0;
	unsigned int mSize = 0;
	unsigned int mBinding = 0;
};
