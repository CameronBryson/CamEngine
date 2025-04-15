#pragma once
#include "Engine/Util/platform.hpp"

class ShaderStorageBuffer
{
public:
	ShaderStorageBuffer(unsigned int size, unsigned int binding);
	~ShaderStorageBuffer();

	ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
	ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;

	ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept;
	ShaderStorageBuffer& operator=(ShaderStorageBuffer&& other) noexcept;

	void bind(unsigned int binding) const;
	void unbind() const;
	void setData(const void* data, unsigned int size, unsigned int offset = 0);
	void clear() ;
	unsigned int getID() const { return mID; }
	unsigned int getSize() const { return mSize; }

private:
	GLuint mID = 0;
	unsigned int mSize = 0;
};

