#pragma once
#include "Engine/Registry.hpp"
#include "Graphics/Camera.hpp"
#include "platform.hpp"

#include <Graphics/GraphicsManager.hpp>
class s_render
{
  public:
    static void init();
    static void update(const registry &registry, Camera &camera);
    static void shutdown();

  private:
    static void draw_statistics();
    static void load_shaders();
    static void load_textures();
};
class object
{
public:
  unsigned int VBO, VAO, EBO = 0;
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  unsigned int index_count;
  void init(const char* file)
  {
    graphics_manager::load_obj(file,vertices,indices);
    index_count = indices.size();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindVertexArray(0);
  }
};
