#pragma once
#include <string>

#include "platform.hpp"

class Texture
{
public:
  explicit Texture(const std::string& file);

  void bind() const;
  static void unbind();
  void deleteTexture() const;
private:
    GLuint texture_id = 0;
    unsigned char* data = nullptr;
};