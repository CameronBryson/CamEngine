#include "OpenGLMesh.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Material.hpp"

OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned> indices, const std::string& materialName) :mIndexCount(vertices.size()), material_name(materialName)
{
	mVertexArray = VertexArray::create();
	mVertexArray->addVertexBuffer(VertexBuffer::create(vertices));
	mVertexArray->setIndexBuffer(IndexBuffer::create(indices));
    //Init VAO
    //Bind VAO
    //Attach VBO to VAO
    //Attach EBO TO VAO

}

OpenGLMesh::~OpenGLMesh()
{

}


void OpenGLMesh::draw(const Shader& shader, GraphicsManager& graphicsManager) const
{
    shader.use();
	mVertexArray->bind();
	graphicsManager.getMaterial(material_name)->bind(shader);

    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
	mVertexArray->unbind();
	graphicsManager.getMaterial(material_name)->unbind();
}

void OpenGLMesh::setMaterial(const std::string& name)
{
	material_name = name;
}
