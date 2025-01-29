#include "pch.hpp"
#include "OpenGLCubemap.hpp"

#include <stb_image.h>
#include <iostream>
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"

OpenGLCubemap::OpenGLCubemap(const std::vector<std::string> facePaths) : cubemapID(0), width(0), height(0)
{
	glGenTextures(1, &cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
    for (unsigned int i = 0; i < facePaths.size(); i++)
    {
        int nrChannels;
        stbi_set_flip_vertically_on_load(false); // Usually don't flip cubemap faces
        unsigned char* data = stbi_load(facePaths[i].c_str(), &width, &height, &nrChannels, 0);

        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 4)
                format = GL_RGBA;

            // Upload this face data
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                format, // internal format
                width, height, 0,
                format,
                GL_UNSIGNED_BYTE,
                data);

            stbi_image_free(data);

            std::cout << "Loaded cubemap face [" << i << "]: " << facePaths[i]
                << " (w=" << width << ", h=" << height << ", channels=" << nrChannels << ")\n";
        }
        else
        {
            std::cerr << "Cubemap face failed to load at path: " << facePaths[i] << std::endl;
            if (data) stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 4) Unbind
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    createSkyboxVAO();
}

OpenGLCubemap::~OpenGLCubemap()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo); // Delete the VBO
    glDeleteTextures(1, &cubemapID);
}


void OpenGLCubemap::draw(const Shader& shader) const
{
    // Change depth function so depth test passes when values are equal to depth buffer's content
    glDepthFunc(GL_LEQUAL);

    shader.use();
    shader.setInt("skybox", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Set depth function back to default
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthFunc(GL_LESS);
}


void OpenGLCubemap::createSkyboxVAO()
{
    // Define the vertices for a cube (skybox)
    float skyboxVertices[] = {
        // Positions          
       -1.0f,  1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,

       -1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

       -1.0f, -1.0f,  1.0f,
       -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,

       -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
       -1.0f,  1.0f,  1.0f,
       -1.0f,  1.0f, -1.0f,

       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    // Generate and bind the VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Unbind the VAO
    glBindVertexArray(0);
}

