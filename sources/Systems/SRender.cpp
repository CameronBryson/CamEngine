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
#include "Graphics/Model.hpp"

void s_render::init()
{
    printf("Render init\n");
    load_shaders();
    graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/Default.mtl");
    graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/patek.mtl");
    //graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/torus.mtl");
    //graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/cone.mtl");
    //graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/test.mtl");
    //graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/goat.mtl");
    graphics_manager::load_mtl("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/triple.mtl");


    //graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/sphere2.obj");
    //graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/cube.obj");
    //graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/goat.obj");
    //graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/torus.obj");
    //graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/cone.obj");
    auto test = graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/triple.obj");



    auto patek = graphics_manager::load_obj("/home/cam/Documents/GitHub/raylib-cmake-template-master/assets/patek.obj");





    graphics_manager::create_model(patek, "player");



    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Specify that we want to cull back faces
    glCullFace(GL_BACK);

    // Optionally, specify the front face winding order (default is GL_CCW)
    glFrontFace(GL_CCW);
}

void s_render::update(const registry & registry, Camera & camera)
{
    graphics_util::clear_background();

    auto & view_matrix = camera.GetViewMatrix();
    auto & proj_matrix = camera.GetProjectionMatrix();
    auto & positions = registry.get_sparse_set<c_transform>();
    auto & shader = graphics_manager::get_shader("vertex");
    auto & models = registry.get_sparse_set<c_model>();
    shader.setMat4("projection", proj_matrix);
    shader.setMat4("view", view_matrix);
    shader.use();
    for( auto id : registry.get_entity_ids<c_model, c_transform>() )
    {
        auto & model = models.get_item(id);
        auto & transform = positions.get_item(id);
        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
            glm::scale(glm::mat4(1.0f), transform.scale) *
            glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
        shader.setMat4("model", model_matrix);
        graphics_manager::get_model(model.name).draw(shader);
    }
    for( auto id : registry.get_entity_ids<c_collider, c_transform>() )
    {
        if( registry.has_component<c_quad>(id) )
        {
            auto & quad = registry.get_component<c_quad>(id);
            auto & transform = registry.get_component<c_transform>(id);
            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
                glm::scale(glm::mat4(1.0f), quad.extents * transform.scale) *
                glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            shader.setMat4("model", model_matrix);

            auto & mesh = graphics_manager::get_mesh("Cube");
            mesh.draw(shader);
        }
        else if( registry.has_component<c_sphere>(id) )
        {
            auto & sphere = registry.get_component<c_sphere>(id);
            auto & transform = registry.get_component<c_transform>(id);
            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
                glm::scale(glm::mat4(1.0f), transform.scale * sphere.radius) *
                glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            shader.setMat4("model", model_matrix);
            auto & mesh = graphics_manager::get_mesh("Sphere");
            mesh.draw(shader);
        }
    }
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
    graphics_util::draw_text(stats_text.c_str(), 50, 0, 50, { 255, 0, 0, 255 });
}

void s_render::load_shaders()
{
    graphics_manager::load_shader(
        "/home/cam/Documents/GitHub/raylib-cmake-template-master/sources/Shaders/vertex_shader.glsl",
        "/home/cam/Documents/GitHub/raylib-cmake-template-master/sources/Shaders/fragment_shader.glsl", "vertex");
}

void s_render::draw_models(const registry & registry, Camera & camera)
{
    //draw the meshes that belong to the models
}

void s_render::draw_colliders(const registry & registry, Camera & camera)
{
    //quads, spheres and capsules
}