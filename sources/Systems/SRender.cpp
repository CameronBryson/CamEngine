#include "SRender.hpp"

#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/Stats.hpp"
#include "Math/MathUtil.hpp"

#include "../Graphics/ShaderProgram.hpp"
#include "Graphics/GraphicsUtil.hpp"

#include <Graphics/GraphicsManager.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
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
    graphics_util::clear_background();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw using shader_program
    auto view_matrix = camera.GetViewMatrix();
    auto proj_matrix = camera.GetProjectionMatrix();
    auto &positions = registry.get_sparse_set<c_transform>();
    auto& shader = graphics_manager::get_shader("vertex");
    shader.setMat4("projection", proj_matrix);
    shader.setMat4("view", view_matrix);
    shader.use();
    auto object = graphics_manager::get_obj("sphere");
    object.enable();
    for(auto sphere_id: registry.get_entity_ids<c_sphere, c_transform>())
    {
        auto &transform = positions.get_item(sphere_id);
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, object.get_count(), GL_UNSIGNED_INT, 0);
    }
    object.disable();
    object = graphics_manager::get_obj("cube");
    object.enable();
    for(auto quad_id: registry.get_entity_ids<c_quad, c_transform>())
    {
        auto &transform = positions.get_item(quad_id);
        auto model = glm::mat4(1.0f);
        model = glm::scale(model, transform.scale);
        model = glm::translate(model, transform.position);
        model = model *  glm::eulerAngleXYZ(transform.rotation.x,transform.rotation.y,transform.rotation.z);
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, object.get_count(), GL_UNSIGNED_INT, 0);
    }
    object.disable();
}

void s_render::shutdown()
{
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