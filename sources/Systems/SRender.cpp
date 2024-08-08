#include "SRender.hpp"

#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/Stats.hpp"
#include "Math/MathUtil.hpp"

#include "../Graphics/ShaderProgram.hpp"
#include "Graphics/GraphicsUtil.hpp"

#include <Graphics/GraphicsManager.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <string>
void s_render::init() {
    printf("Render init\n");
    load_shaders();
    load_textures();
    graphics_manager::load_mesh("C:\\Personal-Programming\\C++\\raylib\\raylib-cmake\\assets\\cube.obj", "cube");
    //graphics_manager::load_mesh("C:\\Personal-Programming\\C++\\raylib\\raylib-cmake\\assets\\chiron.obj", "chiron");
    graphics_manager::load_mesh("C:\\Personal-Programming\\C++\\raylib\\raylib-cmake\\assets\\deagle.obj", "deagle");






}

void s_render::update(const registry &registry, Camera &camera)
{
    graphics_util::clear_background();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    auto view_matrix = camera.GetViewMatrix();
    auto proj_matrix = camera.GetProjectionMatrix();
    auto &positions = registry.get_sparse_set<c_transform>();
    auto& shader = graphics_manager::get_shader("vertex");
    shader.setMat4("projection", proj_matrix);
    shader.setMat4("view", view_matrix);
    shader.use();

    auto draw_object = [&](const std::string &mesh_name, const auto &entity_ids) {
        auto object = graphics_manager::get_mesh(mesh_name);
        for (auto id : entity_ids) {
            auto &transform = positions.get_item(id);
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position) * glm::scale(model, transform.scale) * glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            shader.setMat4("model", model);
            object.draw();
        }
    };

    draw_object("deagle", registry.get_entity_ids<c_sphere, c_transform>());
    draw_object("deagle", registry.get_entity_ids<c_quad, c_transform>());
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
        "C:\\Personal-Programming\\C++\\raylib\\raylib-cmake\\sources\\Shaders\\vertex_shader.glsl",
        "C:\\Personal-Programming\\C++\\raylib\\raylib-cmake\\sources\\Shaders\\fragment_shader.glsl", "vertex");
}
void s_render::load_textures()
{
}
void s_render::draw_models(const registry &registry, Camera &camera)
{
    //draw the meshes that belong to the models
}
void s_render::draw_colliders(const registry &registry, Camera &camera)
{
    //quads, spheres and capsules
}
