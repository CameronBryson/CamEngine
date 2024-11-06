#include "OpenGLUtil.hpp"
#include "GraphicsManager.hpp"
#include "Vertex.hpp"
#include "ShaderProgram.hpp"
#include "stb_image.h"
#include "Material.hpp"
#include <iostream>

void OpenGlUtil::init()
{
    glfwSetErrorCallback(engine_util::errorCallback);
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
    glfwSetKeyCallback(game_window, engine_util::keyCallback);
    glfwSetMouseButtonCallback(game_window, engine_util::mouseKeyCallback);
    glfwSetFramebufferSizeCallback(game_window, OpenGlUtil::framebufferSizeCallback);
    glfwSwapInterval(0);
    gladLoadGL(glfwGetProcAddress);


    glClearColor(1, 1, 1, 1);

    GameManager::set_glfw_window(game_window);
}

void OpenGlUtil::drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color)
{
    convertPointToScreen(start);
    convertPointToScreen(end);
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
}

void OpenGlUtil::drawText(const char* text, int posX, int posY, int fontSize, glm::vec3 color)
{
}

void OpenGlUtil::clearBackground()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec2 OpenGlUtil::convertPointToScreen(glm::vec2& point)
{
    point.x = ((point.x / settings::window_width) * 2.0f) - 1.0f;
    point.y = ((point.y / settings::window_height) * 2.0f) - 1.0f;
    return point;
}

void OpenGlUtil::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGlUtil::deleteShaderProgram(unsigned ID)
{
    glDeleteProgram(ID);
}

void OpenGlUtil::setupMesh(const std::vector<Vertex>& vertices, unsigned int& VAO, unsigned int& VBO)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates));


    glBindVertexArray(0);
}

void OpenGlUtil::drawMesh(const ShaderProgram& shader, GraphicsManager& graphics_manager, const std::string& material_name, unsigned int VAO,
    unsigned int index_count)
{
    graphics_manager.getMaterial(material_name).bind(shader);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, index_count);
    glBindVertexArray(0);
    graphics_manager.getMaterial(material_name).unbind();
}

void OpenGlUtil::bindTexture(GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
}

void OpenGlUtil::unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlUtil::deleteTexture(GLuint texture)
{
    glDeleteTextures(1, &texture);
}

void OpenGlUtil::createTexture(const std::string& file, unsigned char* data, GLuint& texture_id)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(engine_util::buildPath(file).c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        throw std::runtime_error("Failed to load texture: " + std::string(file));
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    unbindTexture();

    stbi_image_free(data);
}

void OpenGlUtil::bindMaterial(const ShaderProgram& shader, Material& material)
{
    // Bind Ambient Texture (map_Ka)
    if( material.map_Ka )
    {
	shader.setInt("material.ambient", 0); // Bind texture unit 0 to ambient map
	glActiveTexture(GL_TEXTURE0);
	material.map_Ka->bind();
    }
    else
    {
	shader.setVec3("material.ambient", material.Ka); // Fallback to ambient color
    }

    // Bind Diffuse Texture (map_Kd)
    if( material.map_Kd )
    {
	shader.setInt("material.diffuse", 1); // Bind texture unit 1 to diffuse map
	glActiveTexture(GL_TEXTURE1);
	material.map_Kd->bind();
    }
    else
    {
	shader.setVec3("material.diffuse", material.Kd); // Fallback to diffuse color
    }

    // Bind Specular Texture (map_Ks)
    if( material.map_Ks )
    {
	shader.setInt("material.specular", 2); // Bind texture unit 2 to specular map
	glActiveTexture(GL_TEXTURE2);
	material.map_Ks->bind();
    }
    else
    {
	shader.setVec3("material.specular", material.Ks); // Fallback to specular color
    }

    // Bind Shininess Texture (map_Ns)
    if( material.map_Ns )
    {
	shader.setInt("material.shininessMap", 3); // Bind texture unit 3 to shininess map
	glActiveTexture(GL_TEXTURE3);
	material.map_Ns->bind();
    }
    else
    {
	shader.setFloat("material.shininess", material.Ns); // Fallback to shininess value
    }

    // Bind Transparency Texture (map_d)
    if( material.map_d )
    {
	shader.setInt("material.transparencyMap", 4); // Bind texture unit 4 to transparency map
	glActiveTexture(GL_TEXTURE4);
	material.map_d->bind();
	shader.setFloat("material.transparency", material.d); // Set transparency factor (d)
    }
    else
    {
	shader.setFloat("material.transparency", material.d); // Fallback to transparency value
    }

    // Bind Bump Map (map_bump)
    if( material.map_bump )
    {
	shader.setInt("material.bumpMap", 5); // Bind texture unit 5 to bump map
	glActiveTexture(GL_TEXTURE5);
	material.map_bump->bind();
    }

    // Set other material properties
    shader.setVec3("material.ambientColor", material.Ka); // Set the ambient color (Ka)
    shader.setVec3("material.diffuseColor", material.Kd); // Set the diffuse color (Kd)
    shader.setVec3("material.specularColor", material.Ks); // Set the specular color (Ks)
    shader.setFloat("material.shininess", material.Ns); // Set shininess (Ns)
}


void OpenGlUtil::unbindMaterial(Material& material)
{
    Texture::unbind();
}

unsigned int OpenGlUtil::createShader(const std::string & vertex_shader, const std::string & fragment_shader)
{
    const char* vShaderCode = vertex_shader.c_str();
    const char* fShaderCode = fragment_shader.c_str();
    // vertex shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkShaderCompileError(vertex, "VERTEX");
    // fragment Shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkShaderCompileError(fragment, "FRAGMENT");
    // shader Program
    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkShaderCompileError(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

void OpenGlUtil::checkShaderCompileError(unsigned shader, std::string type)
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

void OpenGlUtil::useShader(unsigned shader)
{
    glUseProgram(shader);
}

void OpenGlUtil::setShaderBool(unsigned int shader_id, const std::string & name, bool value)
{
    glUniform1i(glGetUniformLocation(shader_id, name.c_str()), (int)value);
}

void OpenGlUtil::setShaderInt(unsigned int shader_id, const std::string & name, int value)
{
    glUniform1i(glGetUniformLocation(shader_id, name.c_str()), value);
}

void OpenGlUtil::setShaderFloat(unsigned int shader_id, const std::string & name, float value)
{
    glUniform1f(glGetUniformLocation(shader_id, name.c_str()), value);
}

void OpenGlUtil::setShaderVec2(unsigned int shader_id, const std::string & name, const glm::vec2 & value)
{
    glUniform2fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void OpenGlUtil::setShaderVec3(unsigned int shader_id, const std::string & name, const glm::vec3 & value)
{
    glUniform3fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void OpenGlUtil::setShaderVec4(unsigned int shader_id, const std::string & name, const glm::vec4 & value)
{
    glUniform4fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void OpenGlUtil::setShaderMat4(unsigned int shader_id, const std::string & name, const glm::mat4 & value)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
glm::vec2 OpenGlUtil::getWindowSize(){
    int width,height;
    glfwGetWindowSize(GameManager::get_glfw_window(), &width, &height);
    return {width,height};
}
glm::vec2 OpenGlUtil::getMousePos(){
    double x, y;
    glfwGetCursorPos(GameManager::get_glfw_window(),&x,&y);
    return {x,y};
}


