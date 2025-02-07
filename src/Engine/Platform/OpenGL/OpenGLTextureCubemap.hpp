#pragma once

#include "platform.hpp"
#include "TextureCubemap.hpp"
#include <memory>
#include <Shader.hpp>
#include <string>

class OpenGLTextureCubemap : public TextureCubemap
{
public:
    explicit OpenGLTextureCubemap(const std::string& hdrPath, const std::shared_ptr<Shader>& equirectangularToCubemapShader);
	explicit OpenGLTextureCubemap(GLuint textureID, int width, int height);
    ~OpenGLTextureCubemap() override;

    void bind(unsigned int slot) const override;
    void unbind(unsigned int slot) override;
    int getWidth() const override;
    int getHeight() const override;

private:
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
};
