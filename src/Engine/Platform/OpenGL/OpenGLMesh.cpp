#include "pch.hpp"
#include "OpenGLMesh.hpp"
#include "Engine/Util/platform.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "glm/ext/matrix_float4x4_precision.hpp"


OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned> indices, const std::shared_ptr<Material>& material) : mMaterial(material)
{
	mVertexArray = VertexArray::create();
	mVertexArray->addVertexBuffer(VertexBuffer::create(vertices));
	mVertexArray->setIndexBuffer(IndexBuffer::create(indices));
}

OpenGLMesh::~OpenGLMesh()
{

}

void OpenGLMesh::draw(glm::mat4 model) const
{
    if (!mMaterial)
        return;

    
	mMaterial->getShader()->use();
	mMaterial->getShader()->setMat4("model", model);

    mVertexArray->bind();
    

    // Retrieve index count from IndexBuffer
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mVertexArray->getIndexBuffer()->getCount()), GL_UNSIGNED_INT, nullptr);

    mVertexArray->unbind();
    mMaterial->unbind();
}



void OpenGLMesh::setMaterial(const std::shared_ptr<Material>& material)
{
	mMaterial = material;
}

