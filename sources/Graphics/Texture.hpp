#pragma once
#include <string>

#include "OpenGLUtil.hpp"
#include "platform.hpp"

class texture
{
public:
    explicit texture(const std::string& file)
    {
        opengl_util::create_texture(file,data,texture_id);
    }

    void bind() const
    {
        opengl_util::bind_texture(texture_id);
    }
    static void unbind()
    {
        opengl_util::unbind_texture();
    }
    void delete_texture() const
    {
        opengl_util::delete_texture(texture_id);
    }

private:
    GLuint texture_id = 0;
    unsigned char* data = nullptr;
};