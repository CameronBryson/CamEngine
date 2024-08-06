#pragma once
#include "platform.hpp"
#include <string>

class shader_program {
public:
    shader_program(const char* vertexPath, const char* fragmentPath);
    void use() const;
    GLuint getID() const;
    void setMat4(const std::string &name, const GLfloat* value) const;

    void setModelMatrix(const GLfloat* value) const;
    void setViewMatrix(const GLfloat* value) const;
    void setProjectionMatrix(const GLfloat* value) const;

private:
    GLuint ID;
    std::string read_file(const char* filePath) const;
    void check_errors(GLuint shader, const std::string &type) const;
};