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
void s_render::init() {
    printf("Render init\n");
    load_shaders();
    load_textures();
    graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/torus.obj", "torus");
    graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/pyramid.obj", "pyramid");
    graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/cube.obj", "cube");
    graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/sphere.obj", "sphere");
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
    auto object = graphics_manager::get_obj("sphere");
    object.enable();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    for (unsigned int i = 0; i <10; i++)
    {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, object.get_count(), GL_UNSIGNED_INT, 0);
    }
    object.disable();
    auto object2 = graphics_manager::get_obj("cube");
    object2.enable();
    for (unsigned int i = 0; i <10; i++)
    {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, object2.get_count(), GL_UNSIGNED_INT, 0);
    }
    object2.disable();
}

void s_render::shutdown()
{
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
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