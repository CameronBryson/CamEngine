#pragma once
#include <memory>
#include <vector>
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Engine/Util/platform.hpp"
class VertexArray
{
public:
	VertexArray();
	~VertexArray() ;
	void bind() const ;
	void unbind() const ;
	void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) ;
	const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const ;

	void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) ;
	const std::shared_ptr<IndexBuffer>& getIndexBuffer() const ;
private:
	GLuint  mVAO;
	std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers;
	std::shared_ptr<IndexBuffer> mIndexBuffer;
};