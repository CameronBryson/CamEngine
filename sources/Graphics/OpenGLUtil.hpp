#pragma once
#include "platform.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <cstring>
struct Vertex;
class Material;
class Texture;
class ShaderProgram;
class GraphicsManager;

class OpenGlUtil
{
public:
    static void init();
    static void drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);

    static void drawText(const char* text, int posX, int posY, int fontSize, glm::vec3 color);

    static void clearBackground();

    static glm::vec2 convertPointToScreen(glm::vec2& point);

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    static void deleteShaderProgram(unsigned ID);

    static void setupMesh(const std::vector<Vertex>& vertices, unsigned int& VAO, unsigned int& VBO);

    static void drawMesh(const ShaderProgram& shader,GraphicsManager& graphics_manager, const std::string& material_name, unsigned int VAO, unsigned int index_count);

    static void bindTexture(GLuint texture);

    static void unbindTexture();

    static void deleteTexture(GLuint texture);

    static void createTexture(const std::string& file, unsigned char* data, GLuint& texture_id);

    static void bindMaterial(const ShaderProgram& shader, const Texture* diffuse_texture, const Texture* specular_texture, glm::vec3 ambient_color, glm::vec3 diffuse_color, glm::vec3 specular_color, float shininess);

    static void unbindMaterial(const Texture* diffuse_texture, const Texture* specular_texture);

    static unsigned int createShader(const std::string& vertex_shader, const std::string& fragment_shader);

    static void checkShaderCompileError(unsigned shader, std::string type);

    static void useShader(unsigned shader);

    static void setShaderBool(unsigned int shader_id, const std::string& name, bool value);

    static void setShaderInt(unsigned int shader_id, const std::string& name, int value);

    static void setShaderFloat(unsigned int shader_id, const std::string& name, float value);

    static void setShaderVec2(unsigned int shader_id, const std::string& name, const glm::vec2& value);

    static void setShaderVec3(unsigned int shader_id, const std::string& name, const glm::vec3& value);

    static void setShaderVec4(unsigned int shader_id, const std::string& name, const glm::vec4& value);

    static void setShaderMat4(unsigned int shader_id, const std::string& name, const glm::mat4& value);

    static glm::vec2 getWindowSize();

    static glm::vec2 getMousePos();

};


