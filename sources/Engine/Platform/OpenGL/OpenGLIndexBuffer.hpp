#pragma once
#include "Engine/Graphics/IndexBuffer.hpp"
#include <memory>
#include "Engine/Util/platform.hpp"
class OpenGLIndexBuffer : public IndexBuffer
{
public:
	OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
	~OpenGLIndexBuffer();
	void bind() const override;
	void unbind() const override;
	unsigned int getCount() const override;
private:
	GLuint mIBO;
	unsigned int mCount;
};