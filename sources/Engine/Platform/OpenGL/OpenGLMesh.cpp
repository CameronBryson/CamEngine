#include "OpenGLMesh.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Material.hpp"

OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::string& materialName) : vertices(vertices), material_name(materialName)
{
	index_count = vertices.size();
	OpenGLMesh::setupMesh();
}

void OpenGLMesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates));


    glBindVertexArray(0);


}

void OpenGLMesh::draw(const Shader& shader, GraphicsManager& graphicsManager) const
{
	graphicsManager.getMaterial(material_name)->bind(shader);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, index_count);
	glBindVertexArray(0);
	graphicsManager.getMaterial(material_name)->unbind();
}

void OpenGLMesh::setMaterial(const std::string& name)
{
	material_name = name;
}
