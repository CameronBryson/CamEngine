#pragma once

#include <memory>
#include <string>
#include "platform.hpp"
#include <Texture.hpp>

class Shader;
class TextureCubemap : public Texture
{
public:
    TextureCubemap(const std::string& hdrPath,
                         const std::shared_ptr<Shader>& equirectShader);

    // Creates an empty cubemap of width x height. 
    TextureCubemap(GLuint id, int width, int height);

    ~TextureCubemap() override;

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
