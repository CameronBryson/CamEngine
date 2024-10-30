#pragma once
#include <string>

#include "OpenGLUtil.hpp"

class texture
{
public:
  explicit texture(const std::string& file);

  void bind() const;
  static void unbind();
  void delete_texture() const;
private:
    GLuint texture_id = 0;
    unsigned char* data = nullptr;
};