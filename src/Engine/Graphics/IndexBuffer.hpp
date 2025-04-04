#pragma once
#include <memory>
#include <vector>
#include "Engine/Util/platform.hpp"
class IndexBuffer
{
public:
	IndexBuffer(const std::vector<unsigned>& indices);
	~IndexBuffer();
	void bind() const;
	void unbind() const;
	unsigned int getCount() const;
private:
	GLuint mEBO;
	unsigned int mCount;
};