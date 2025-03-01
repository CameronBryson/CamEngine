#pragma once
#include "ShaderStorageBuffer.hpp"
class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
{
public:
	OpenGLShaderStorageBuffer(unsigned int size, unsigned int binding);
	virtual ~OpenGLShaderStorageBuffer() override;
	virtual void bind(unsigned int binding) const override;
	virtual void unbind() const override;
	virtual void setData(const void* data, unsigned int size) override;
	virtual unsigned int getID() const override;
	virtual void clear() override;
private:
	unsigned int m_ID;
	unsigned int m_Size;
};

