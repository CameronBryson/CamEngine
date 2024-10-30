#pragma once
#include <utility>
#include <vector>
#include "Mesh.hpp"
#include "GraphicsManager.hpp"
class model
{
public:
    explicit model(const std::vector<std::string>& meshes)
    {
        for (const auto& mesh_name : meshes)
        {
            add_mesh(mesh_name);
        }
    }

    void draw(shader_program& shader, graphics_manager& graphics_manager)
    {
        for (auto& mesh : meshes)
        {
            graphics_manager.get_mesh(mesh).draw(shader,graphics_manager);
        }
    }

    void add_mesh(const std::string& mesh_name)
    {
        meshes.push_back(mesh_name);
    }

private:
    std::vector<std::string> meshes;
};
