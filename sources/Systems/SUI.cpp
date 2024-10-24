
#include "SUI.hpp"
#include "Engine/GameManager.hpp"
#include "Graphics/OpenGLUtil.hpp"
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
            glm::vec2 mouse_pos = opengl_util::get_mouse_pos();
            glm::vec2 window_size = opengl_util::get_window_size();
            float x = (mouse_pos.x/window_size.x) * 2.0f -1.0f;
            float y = -(mouse_pos.y / window_size.y) * 2.0+1.0f;
            transform.position.x = x;
            transform.position.y = y;
        }

    }
}
void s_ui::shutdown()
{
}
