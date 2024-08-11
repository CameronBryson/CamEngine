#pragma once
#include <string>
#include <stdexcept>
#include "stb_image.h"
#include "platform.hpp"

class texture {
public:
  texture(const char* file, bool alpha) {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data) {
      throw std::runtime_error("Failed to load texture: " + std::string(file));
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
  }

  ~texture() {
    glDeleteTextures(1, &texture_id);
  }

  void bind() const {
    glBindTexture(GL_TEXTURE_2D, texture_id);
  }

private:
  GLuint texture_id;
  unsigned char* data;
};