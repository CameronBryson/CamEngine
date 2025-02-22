#pragma once

#include "Engine/Graphics/Abstract/TextureCubemap.hpp"
#include <memory>
#include <string>
#include "platform.hpp"

class Shader;
class OpenGLTextureCubemap : public TextureCubemap
{
public:
    OpenGLTextureCubemap(const std::string& hdrPath,
        const std::shared_ptr<Shader>& equirectShader);

    // Creates an empty cubemap of width x height. 
    OpenGLTextureCubemap(GLuint id, int width, int height);

    ~OpenGLTextureCubemap() override;

    unsigned int  getTextureID() const  override { return textureID; }

    void bind(unsigned int slot)override;
    void unbind(unsigned int slot) override;
    int  getWidth()  const override;
    int  getHeight() const override;
	void setShadowSamplerParameters() override;
	void setNormalSamplerParameters() override;

private:
    GLuint textureID = 0;
    int    width = 0;
    int    height = 0;
	bool  isShadowSampler = false;

};
