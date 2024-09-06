#include "OpenGLUtil.hpp"
#include "GraphicsManager.hpp"
#include "Vertex.hpp"
#include "ShaderProgram.hpp"
#include "stb_image.h"
#include "Material.hpp"

void opengl_util::init()
{
    glfwSetErrorCallback(engine_util::error_callback);
    if( ! glfwInit() )
    {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* game_window = glfwCreateWindow(settings::window_width, settings::window_height, "Game Window", nullptr, nullptr);
    if( ! game_window )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(game_window);
    glfwSetKeyCallback(game_window, engine_util::key_callback);
    glfwSetFramebufferSizeCallback(game_window, opengl_util::framebuffer_size_callback);
    glfwSwapInterval(0);
    gladLoadGL(glfwGetProcAddress);


    glClearColor(1, 1, 1, 1);

    game_manager::set_glfw_window(game_window);
}

void opengl_util::draw_line(glm::vec2 start, glm::vec2 end, glm::vec3 color)
{
    convert_point_to_screen(start);
    convert_point_to_screen(end);
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
}

void opengl_util::draw_text(const char* text, int posX, int posY, int fontSize, glm::vec3 color)
{
}

void opengl_util::clear_background()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec2 opengl_util::convert_point_to_screen(glm::vec2& point)
{
    point.x = ((point.x / settings::window_width) * 2.0f) - 1.0f;
    point.y = ((point.y / settings::window_height) * 2.0f) - 1.0f;
    return point;
}

void opengl_util::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void opengl_util::delete_shader_program(unsigned ID)
{
    glDeleteProgram(ID);
}

void opengl_util::setup_mesh(const std::vector<vertex>& vertices, unsigned int& VAO, unsigned int& VBO)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texture_coordinates));


    glBindVertexArray(0);
}

void opengl_util::draw_mesh(const shader_program& shader, const std::string& material_name, unsigned int VAO,
    unsigned int index_count)
{
    graphics_manager::get_material(material_name).bind(shader);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, index_count);
    glBindVertexArray(0);
    graphics_manager::get_material(material_name).unbind();
}

void opengl_util::bind_texture(GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
}

void opengl_util::unbind_texture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void opengl_util::delete_texture(GLuint texture)
{
    glDeleteTextures(1, &texture);
}

void opengl_util::create_texture(const std::string& file, unsigned char* data, GLuint& texture_id)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(engine_util::build_path(file).c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        throw std::runtime_error("Failed to load texture: " + std::string(file));
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind_texture();

    stbi_image_free(data);
}

void opengl_util::bind_material(const shader_program & shader, const texture * diffuse_texture, const texture * specular_texture, const glm::vec3 ambient_color,
    const glm::vec3 diffuse_color, const glm::vec3 specular_color, const float shininess)
{
    if (diffuse_texture)
    {
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        diffuse_texture->bind();
    }
    else
    {
        shader.setVec3("material.diffuse", diffuse_color);
    }

    if (specular_texture)
    {
        shader.setInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE1);
        specular_texture->bind();
    }
    else
    {
        shader.setVec3("material.specular", specular_color);
    }

    shader.setFloat("material.shininess", shininess);
    shader.setVec3("material.ambient", ambient_color);
}

void opengl_util::unbind_material(const texture * diffuse_texture, const texture * specular_texture)
{
    if(diffuse_texture)
    {
        texture::unbind();
    }
    if(specular_texture)
    {
        texture::unbind();
    }
}

unsigned int opengl_util::create_shader(const std::string & vertex_shader, const std::string & fragment_shader)
{
    const char* vShaderCode = vertex_shader.c_str();
    const char* fShaderCode = fragment_shader.c_str();
    // vertex shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    check_shader_compile_error(vertex, "VERTEX");
    // fragment Shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    check_shader_compile_error(fragment, "FRAGMENT");
    // shader Program
    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    check_shader_compile_error(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

void opengl_util::check_shader_compile_error(unsigned shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void opengl_util::use_shader(unsigned shader)
{
    glUseProgram(shader);
}

void opengl_util::set_shader_bool(unsigned int shader_id, const std::string & name, bool value)
{
    glUniform1i(glGetUniformLocation(shader_id, name.c_str()), (int)value);
}

void opengl_util::set_shader_int(unsigned int shader_id, const std::string & name, int value)
{
    glUniform1i(glGetUniformLocation(shader_id, name.c_str()), value);
}

void opengl_util::set_shader_float(unsigned int shader_id, const std::string & name, float value)
{
    glUniform1f(glGetUniformLocation(shader_id, name.c_str()), value);
}

void opengl_util::set_shader_vec2(unsigned int shader_id, const std::string & name, const glm::vec2 & value)
{
    glUniform2fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void opengl_util::set_shader_vec3(unsigned int shader_id, const std::string & name, const glm::vec3 & value)
{
    glUniform3fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void opengl_util::set_shader_vec4(unsigned int shader_id, const std::string & name, const glm::vec4 & value)
{
    glUniform4fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void opengl_util::set_shader_mat4(unsigned int shader_id, const std::string & name, const glm::mat4 & value)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}


