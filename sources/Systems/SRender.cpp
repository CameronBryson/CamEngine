#include "SRender.hpp"

#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/Stats.hpp"
#include "Math/MathUtil.hpp"

#include "Graphics/OpenGLUtil.hpp"

#include <Graphics/GraphicsManager.hpp>
#include "Graphics/Model.hpp"
#include "Graphics/Camera.hpp"
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

void s_render::init(graphics_manager& graphics_manager)
{
    printf("Render init\n");
    load_shaders(graphics_manager);
    graphics_manager.load_mtl("assets/Default.mtl");
    //graphics_manager::create_model_from_obj("assets/cube.obj", "player");
    graphics_manager.create_model_from_obj("assets/Ship.obj", "player");

    //graphics_manager::create_model_from_obj("assets/patek.obj", "player");
    graphics_manager.create_model_from_obj("assets/sphere.obj", "sphere");
    graphics_manager.create_model_from_obj("assets/cube.obj", "cube");
    graphics_manager.create_model_from_obj("assets/quad.obj", "quad");
    graphics_manager.create_model_from_obj("assets/skybox.obj", "skybox");
    graphics_manager.create_model_from_obj("assets/asteroid.obj", "asteroid");

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Specify that we want to cull back faces
    glCullFace(GL_BACK);

    // Optionally, specify the front face winding order (default is GL_CCW)
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
}

void s_render::update(const registry & registry, graphics_manager& graphics_manager,  Camera & camera)
{
    opengl_util::clear_background();

    auto & view_matrix = camera.GetViewMatrix();
    auto & proj_matrix = camera.GetProjectionMatrix();
    auto & transforms = registry.get_sparse_set<c_transform>();
    auto & texture_shader_3D = graphics_manager.get_shader("3D_texture");
    auto & color_shader_3D = graphics_manager.get_shader("3D_color");
    auto & color_shader_2D = graphics_manager.get_shader("2D_color");
    auto & texture_shader_2D = graphics_manager.get_shader("2D_texture");
    auto & direction_lights = registry.get_sparse_set<c_directional_light>();
    auto & point_lights = registry.get_sparse_set<c_point_light>();
    const auto direction_light_ids = registry.get_entity_ids<c_directional_light>();
    const auto point_light_ids = registry.get_entity_ids<c_point_light,c_transform>();


    glm::mat4 ortho_projection = glm::ortho(-settings::aspect_ratio, settings::aspect_ratio, -1.0f, 1.0f, -1.0f, 1.0f);


    glEnable(GL_DEPTH_TEST);


    texture_shader_3D.use();
    texture_shader_3D.setMat4("projection", proj_matrix);
    texture_shader_3D.setMat4("view", view_matrix);
    texture_shader_3D.setInt("numDirLights", direction_light_ids.size());
    texture_shader_3D.setInt("numPointLights", point_light_ids.size());
    texture_shader_3D.setVec3("viewPos", camera.Position);

    for( int i = 0; i < direction_light_ids.size(); ++i )
    {
        auto & light = direction_lights.get_item(direction_light_ids[i]);
        std::string index = std::to_string(i);
        texture_shader_3D.setVec3("dirLights[" + index + "].direction", light.direction);
        texture_shader_3D.setVec3("dirLights[" + index + "].ambient", light.ambient);
        texture_shader_3D.setVec3("dirLights[" + index + "].diffuse", light.diffuse);
        texture_shader_3D.setVec3("dirLights[" + index + "].specular", light.specular);
    }
    for( int i = 0; i < point_light_ids.size(); ++i )
    {
        auto & light = point_lights.get_item(point_light_ids[i]);
        auto & transform = transforms.get_item(point_light_ids[i]);
        std::string index = std::to_string(i);
        texture_shader_3D.setVec3("pointLights[" + index + "].position", transform.position);
        texture_shader_3D.setVec3("pointLights[" + index + "].ambient", light.ambient);
        texture_shader_3D.setVec3("pointLights[" + index + "].diffuse", light.diffuse);
        texture_shader_3D.setVec3("pointLights[" + index + "].specular", light.specular);
        texture_shader_3D.setFloat("pointLights[" + index + "].constant", light.constant);
        texture_shader_3D.setFloat("pointLights[" + index + "].linear", light.linear);
        texture_shader_3D.setFloat("pointLights[" + index + "].quadratic", light.quadratic);

    }
    draw_models(registry, graphics_manager, transforms, texture_shader_3D);
//#ifdef _DEBUG
    color_shader_3D.use();
    color_shader_3D.setMat4("projection", proj_matrix);
    color_shader_3D.setMat4("view", view_matrix);
    draw_colliders(registry, graphics_manager, transforms, color_shader_3D);
//#endif



    color_shader_2D.use();

    color_shader_2D.setMat4("projection", ortho_projection);
    color_shader_2D.setMat4("view", glm::mat4(1.0f));
    draw_ui(registry, graphics_manager, transforms, color_shader_2D);
}

void s_render::shutdown()
{
}

void s_render::draw_models(const registry & registry,graphics_manager& graphics_manager, sparse_set<c_transform> & transforms, shader_program & shader)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    auto& models = registry.get_sparse_set<c_model>();
    for( auto id : registry.get_entity_ids<c_model, c_transform,c_background>() )
    {
        auto & model = models.get_item(id);
        auto & transform = transforms.get_item(id);
        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
            glm::scale(glm::mat4(1.0f), transform.scale) *
            glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
        shader.setMat4("model", model_matrix);
        graphics_manager.get_model(model.name).draw(shader,graphics_manager);
    }
    for( auto id : registry.get_entity_ids<c_model, c_transform>() )
    {
        auto & model = models.get_item(id);
        auto & transform = transforms.get_item(id);
        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
            glm::scale(glm::mat4(1.0f), transform.scale) *
            glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
        shader.setMat4("model", model_matrix);
        graphics_manager.get_model(model.name).draw(shader,graphics_manager);
    }

}

void s_render::draw_colliders(const registry & registry, graphics_manager& graphics_manager, sparse_set<c_transform> & transforms, shader_program & shader)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    auto & quads = registry.get_sparse_set<c_quad>();
    auto & spheres = registry.get_sparse_set<c_sphere>();
    for( auto id : registry.get_entity_ids<c_collider, c_transform>())
    {
        if( registry.has_component<c_quad>(id) )
        {
            auto & quad = quads.get_item(id);
            auto & transform = transforms.get_item(id);
            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
                glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z)
                * glm::scale(glm::mat4(1.0f), quad.extents * transform.scale);
            shader.setMat4("model", model_matrix);

            auto & mesh = graphics_manager.get_mesh("Cube");
            mesh.draw(shader,graphics_manager);
        }
        else if( registry.has_component<c_sphere>(id) )
        {
            auto & sphere = spheres.get_item(id);
            auto & transform = transforms.get_item(id);
            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
                glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z) *
                glm::scale(glm::mat4(1.0f), transform.scale * (sphere.radius*2));
            shader.setMat4("model", model_matrix);
            auto & mesh = graphics_manager.get_mesh("Sphere");
            mesh.draw(shader,graphics_manager);
        }
    }
}

void s_render::draw_ui(const registry &registry, graphics_manager& graphics_manager, sparse_set<c_transform> &transforms, shader_program &shader) {
    glDisable(GL_DEPTH_TEST);

    auto &ui = registry.get_sparse_set<c_ui>();
    for (auto id : registry.get_entity_ids<c_ui, c_transform>()) {
        auto &transform = transforms.get_item(id);
        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
                                 glm::scale(glm::mat4(1.0f), transform.scale) *
                                 glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);
        shader.setMat4("model", model_matrix);
        auto &mesh = graphics_manager.get_mesh("Quad");
        mesh.draw(shader,graphics_manager);
    }
}


void s_render::load_shaders(graphics_manager& graphics_manager)
{
    graphics_manager.load_shader(
        "sources/Shaders/vertex.vs",
        "sources/Shaders/3D_texture.fs", "3D_texture");
    graphics_manager.load_shader(
            "sources/Shaders/vertex.vs",
            "sources/Shaders/3D_color.fs", "3D_color");
    graphics_manager.load_shader(
        "sources/Shaders/vertex.vs",
        "sources/Shaders/2D_color.fs", "2D_color");
    graphics_manager.load_shader(
            "sources/Shaders/vertex.vs",
            "sources/Shaders/2D_color.fs", "2D_color");
}
