#pragma once
#include "VertexArray.hpp"
#include "Engine/Util/platform.hpp"
class OpenGLVertexArray : public VertexArray
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray() override;
	void bind() const override;
	void unbind() const override;
	void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
	const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override;

	void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
	const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override;
private:
	GLuint  mVAO;
	std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers;
	std::shared_ptr<IndexBuffer> mIndexBuffer;
};