#pragma once
#include <memory>
#include <vector>
class IndexBuffer
{
public:
	virtual ~IndexBuffer() = default;
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
	virtual unsigned int getCount() const = 0;
	static std::shared_ptr<IndexBuffer> create(const std::vector<unsigned>& indices);
};