#include "SRender.hpp"

#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/Stats.hpp"
#include "Graphics/Faces.hpp"
#include "Math/MathUtil.hpp"

#include "../Graphics/ShaderProgram.hpp"
#include "Graphics/GraphicsUtil.hpp"

#include <Graphics/GraphicsManager.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

// world space positions of our cubes
    static glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // A 0
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // B 1
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // C 2
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // D 3
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // E 4
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   // F 5
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   // G 6
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   // H 7

        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  // D 8
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // A 9
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // E 10
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // H 11
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   // B 12
        0.5f,  0.5f, -0.5f,  1.0f, 0.0f,   // C 13
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   // G 14
        0.5f, -0.5f,  0.5f,  0.0f, 1.0f,   // F 15

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // A 16
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   // B 17
        0.5f, -0.5f,  0.5f,  1.0f, 1.0f,   // F 18
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // E 19
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  // C 20
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  // D 21
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // H 22
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f,  // G 23
    };
    static const unsigned int indices[] = {
        // front and back
        0, 3, 2,
        2, 1, 0,
        4, 5, 6,
        6, 7 ,4,
        // left and right
        11, 8, 9,
        9, 10, 11,
        12, 13, 14,
        14, 15, 12,
        // bottom and top
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20
    };
unsigned int VBO, VAO,EBO;
// Update the init function to initialize both shader programs
void s_render::init() {
    printf("Render init\n");
    load_shaders();
    load_textures();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindVertexArray(0);
}

// Update the update function to draw twice
void s_render::update(const registry &registry, Camera &camera)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw using shader_program
    auto view_matrix = camera.GetViewMatrix();
    auto proj_matrix = camera.GetProjectionMatrix();
    auto &positions = registry.get_sparse_set<c_transform>();
    auto &quads = registry.get_sparse_set<c_quad>();
    auto& shader = graphics_manager::get_shader("vertex");
    shader.setMat4("projection", proj_matrix);
    shader.setMat4("view", view_matrix);
    shader.use();
    glBindVertexArray(VAO);
    for (unsigned int i = 0; i <10; i++)
    {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void s_render::shutdown()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void s_render::draw_statistics()
{
    const std::string stats_text = //"FPS: " + std::to_string(engine_util::get_fps()) + "\n" +
                                   //"DeltaTime: " + std::to_string(engine_util::get_deltatime()) + "\n" +
        "Update: " + std::to_string(stats::timer_vector[stats::stat_type::UPDATE].count()) + " MS\n" +
        "Render: " + std::to_string(stats::timer_vector[stats::stat_type::RENDER].count()) + " MS\n" +
        "Benchmark: " + std::to_string(stats::timer_vector[stats::stat_type::BENCHMARK].count()) + " MS";
    graphics_util::draw_text(stats_text.c_str(), 50, 0, 50, {255, 0, 0, 255});
}
void s_render::load_shaders()
{
    graphics_manager::load_shader(
        "/home/cam/Documents/GitHub/raylib-cmake-template-master/sources/Shaders/vertex_shader.glsl",
        "/home/cam/Documents/GitHub/raylib-cmake-template-master/sources/Shaders/fragment_shader.glsl", "vertex");
}
void s_render::load_textures()
{
}