#pragma once
#include <vector>
#include "GraphicsManager.hpp"
class model
{
  public:
    void draw()
    {
        for (auto &mesh : meshes)
        {
            mesh.draw();
        }
    }
    void add_mesh(const mesh &m)
    {
        meshes.push_back(m);
    }
  private:
    std::vector<mesh> meshes;
};
