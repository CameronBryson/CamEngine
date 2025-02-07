#include "pch.hpp"
#include "OpenGLTextureCubemap.hpp"
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "platform.hpp"
#include <vector>
#include "Shader.hpp"
#include <MeshFactory.hpp>

OpenGLTextureCubemap::OpenGLTextureCubemap(const std::string& hdrPath, const std::shared_ptr<Shader>& equirectangularToCubemapShader)
{
    // Load HDR equirectangular environment map
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(hdrPath.c_str(), &width, &height, nullptr, 0);
    if (!data)
    {
        std::cerr << "Failed to load HDR image: " << hdrPath << std::endl;
        return;
    }

    
}

OpenGLTextureCubemap::OpenGLTextureCubemap(GLuint textureID, int width, int height)
{
}

OpenGLTextureCubemap::~OpenGLTextureCubemap()
{
    //Delete here
}

void OpenGLTextureCubemap::bind(unsigned int slot) const
{

}

void OpenGLTextureCubemap::unbind(unsigned int slot)
{

}


int OpenGLTextureCubemap::getWidth() const
{
    return width;
}

int OpenGLTextureCubemap::getHeight() const
{
    return height;
}

