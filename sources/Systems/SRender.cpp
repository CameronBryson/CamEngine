#include "SRender.hpp"

#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/Stats.hpp"
#include "Graphics/Faces.hpp"
#include "Math/MathUtil.hpp"

#include "Graphics/GraphicsUtil.hpp"
#include "Shaders/ShaderProgram.hpp"

#include <string>
static shader_program* shader;
static GLuint quadVAO, quadVBO, quadEBO;
static GLuint sphereVAO, sphereVBO, sphereEBO;

void s_render::init()
{
    printf("Render init\n");
    shader = new shader_program("/home/cam/Documents/GitHub/raylib-cmake-template-master/sources/Shaders/vertex_shader.glsl", "/home/cam/Documents/GitHub/raylib-cmake-template-master/sources/Shaders/fragment_shader.glsl");

    // Set up quad vertex data and buffers
    std::vector<float> quadVertices = faces::get_quad_vertices();
    std::vector<unsigned int> quadIndices = faces::get_quad_indices();

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(unsigned int), quadIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Set up sphere vertex data and buffers
    std::vector<float> sphereVertices = faces::get_sphere_vertices(10, 10);
    std::vector<unsigned int> sphereIndices = faces::get_sphere_indices(10, 10);

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void s_render::update(const registry &registry, const camera &camera)
{
    graphics_util::clear_background();
    const auto view_matrix = camera.view_matrix;
    const auto projection_matrix = camera.projection_matrix;

    auto &positions = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();
    auto &aabbs = registry.get_sparse_set<c_quad>();
    shader->use();
    shader->setViewMatrix(view_matrix.data());
    shader->setProjectionMatrix(projection_matrix.data());


    const auto sphereIndicesSize = faces::get_sphere_indices(10, 10).size();
    const auto quadIndicesSize = faces::get_quad_indices().size();

    for (const auto id : registry.get_entity_ids<c_sphere>())
    {
        auto &[radius] = spheres.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id);

        mat4 model_matrix = mat4::create_translation_matrix(position) * mat4::create_rotation_matrix(rotation) *
                           mat4::create_scale_matrix(scale);

        shader->setModelMatrix(model_matrix.data());
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_LINES, sphereIndicesSize, GL_UNSIGNED_INT, 0);
    }

    for (const auto id : registry.get_entity_ids<c_quad>())
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id);
        mat4 translation_matrix = mat4::create_translation_matrix(position);
        mat4 rotation_matrix = mat4::create_rotation_matrix(rotation);
        mat4 scale_matrix = mat4::create_scale_matrix(scale);

        mat4 model_matrix = mat4::create_translation_matrix(position) * mat4::create_rotation_matrix(rotation) *
                           mat4::create_scale_matrix(scale);
        mat4 test_matrix = mat4::create_identity_matrix();

        shader->setModelMatrix(model_matrix.data());
        glBindVertexArray(quadVAO);
        glDrawElements(GL_LINES, quadIndicesSize, GL_UNSIGNED_INT, 0);
    }
}

void s_render::shutdown()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);

    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);

    delete shader;
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