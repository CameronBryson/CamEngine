#pragma once
#include "platform.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <cstring>
struct vertex;
class material;
class texture;
class shader_program;

class opengl_util
{
public:
    static void init();
    static void draw_line(glm::vec2 start, glm::vec2 end, glm::vec3 color);

    static void draw_text(const char* text, int posX, int posY, int fontSize, glm::vec3 color);

    static void clear_background();

    static glm::vec2 convert_point_to_screen(glm::vec2& point);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static void delete_shader_program(unsigned ID);

    static void setup_mesh(const std::vector<vertex>& vertices, unsigned int& VAO, unsigned int& VBO);

    static void draw_mesh(const shader_program& shader, const std::string& material_name, unsigned int VAO, unsigned int index_count);

    static void bind_texture(GLuint texture);

    static void unbind_texture();

    static void delete_texture(GLuint texture);

    static void create_texture(const std::string& file, unsigned char* data, GLuint& texture_id);

    static void bind_material(const shader_program& shader, const texture* diffuse_texture, const texture* specular_texture, glm::vec3 ambient_color, glm::vec3 diffuse_color, glm::vec3 specular_color, float shininess);

    static void unbind_material(const texture* diffuse_texture, const texture* specular_texture);

    static unsigned int create_shader(const std::string& vertex_shader, const std::string& fragment_shader);

    static void check_shader_compile_error(unsigned shader, std::string type);

    static void use_shader(unsigned shader);

    static void set_shader_bool(unsigned int shader_id, const std::string& name, bool value);

    static void set_shader_int(unsigned int shader_id, const std::string& name, int value);

    static void set_shader_float(unsigned int shader_id, const std::string& name, float value);

    static void set_shader_vec2(unsigned int shader_id, const std::string& name, const glm::vec2& value);

    static void set_shader_vec3(unsigned int shader_id, const std::string& name, const glm::vec3& value);

    static void set_shader_vec4(unsigned int shader_id, const std::string& name, const glm::vec4& value);

    static void set_shader_mat4(unsigned int shader_id, const std::string& name, const glm::mat4& value);

    static glm::vec2 get_window_size();

    static glm::vec2 get_mouse_pos();

};


