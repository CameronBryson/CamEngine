#include "ShaderProgram.hpp"
#include "platform.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

shader_program::shader_program(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = read_file(vertexPath);
    std::string fragmentCode = read_file(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    check_errors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    check_errors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    check_errors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void shader_program::use() const {
    glUseProgram(ID);
}

GLuint shader_program::getID() const {
    return ID;
}

void shader_program::setMat4(const std::string &name, const GLfloat* value) const {
    GLuint matrix_id = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, value);
}

void shader_program::setModelMatrix(const GLfloat* value) const {
    setMat4("model_matrix", value);
}

void shader_program::setViewMatrix(const GLfloat* value) const {
    setMat4("view_matrix", value);
}

void shader_program::setProjectionMatrix(const GLfloat* value) const {
    setMat4("projection_matrix", value);
}

std::string shader_program::read_file(const char* filePath) const {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void shader_program::check_errors(GLuint shader, const std::string &type) const {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
}