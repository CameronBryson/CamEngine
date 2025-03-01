#pragma once
#include <memory>
class ShaderStorageBuffer
{
public:
	virtual ~ShaderStorageBuffer() = default;

	virtual void bind(unsigned int binding) const = 0;
	virtual void unbind() const = 0;
	virtual void setData(const void* data, unsigned int size) = 0;
	virtual unsigned int getID() const = 0;
	virtual void clear() = 0;

	static std::shared_ptr<ShaderStorageBuffer> create(unsigned int size, unsigned int binding);
};

