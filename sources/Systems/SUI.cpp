
#include "SUI.hpp"
#include "Engine/GameManager.hpp"
void s_ui::init()
{
}
void s_ui::update(const registry & registry)
{
    auto & transforms = registry.get_sparse_set<c_transform>();
    auto & ui = registry.get_sparse_set<c_ui>();
    for( auto id : registry.get_entity_ids<c_ui, c_transform>() )
    {
        if(ui.get_item(id).follow_cursor){
            auto & transform = transforms.get_item(id);
            double x, y;
            glfwGetCursorPos(game_manager::get_glfw_window(),&x,&y);
            x = (x / settings::window_width) * 2.0 * settings::aspect_ratio - settings::aspect_ratio;
            y = 1.0 - (y / settings::window_height) * 2.0;
            transform.position.x = x;
            transform.position.y = y;
        }

    }
}
void s_ui::shutdown()
{
}
