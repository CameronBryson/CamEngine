
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
            int width,height;
            glfwGetWindowSize(game_manager::get_glfw_window(), &width, &height);

            float aspect_ratio = (float)width/(float)height;
            //x = (x / width) * 2.0 * aspect_ratio - aspect_ratio;
            x = (x/width) * 2.0f -1.0f;
            y = -(y / height) * 2.0+1.0f;
            transform.position.x = x;
            transform.position.y = y;
        }

    }
}
void s_ui::shutdown()
{
}
