#pragma once
#include <utility>
#include <vector>
#include "Mesh.hpp"

class model
{
public:
    explicit model(const std::vector<std::string>& meshes)
    {
        for (const auto& mesh_name : meshes)
        {
            add_mesh(graphics_manager::get_mesh(mesh_name));
        }
    }

    void draw(shader_program& shader)
    {
        for (auto& mesh : meshes)
        {
            mesh.draw(shader);
        }
    }

    void add_mesh(const mesh& m)
    {
        meshes.push_back(m);
    }

private:
    std::vector<mesh> meshes;
};
