#pragma once
#include "Engine/Graphics/VertexBuffer.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include <vector>
class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer(const std::vector<Vertex>& vertices);
	~OpenGLVertexBuffer() override;
	void bind() const override;
	void unbind() const override;
	void setData(const void* data, unsigned int size) override;
private:
	GLuint mVBO;
};