#pragma once
#include <memory>
class IndexBuffer
{
public:
	virtual ~IndexBuffer() = default;
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
	virtual unsigned int getCount() const = 0;
	static std::shared_ptr<IndexBuffer> create(unsigned int* indices, unsigned int count);
};