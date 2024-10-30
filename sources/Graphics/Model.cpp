#include "Model.hpp"
#include "Mesh.hpp"
model::model(const std::vector <std::string>& meshes)
{
    for( const auto& mesh_name : meshes )
    {
	add_mesh(mesh_name);
    }
}
void model::draw(shader_program& shader, graphics_manager& graphics_manager)
{
    for( auto& mesh : meshes )
    {
	graphics_manager.get_mesh(mesh).draw(shader, graphics_manager);
    }
}
void model::add_mesh(const std::string& mesh_name)
{
    meshes.push_back(mesh_name);
}
