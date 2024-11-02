
#include "SUI.hpp"
#include "Engine/GameManager.hpp"
#include "Graphics/OpenGLUtil.hpp"
void SUI::init()
{
}
void SUI::update(const Registry & registry)
{
    auto & transforms = registry.getSparseSet<CTransform>();
    auto & ui = registry.getSparseSet<CUI>();
    for( auto id : registry.getEntityIDs<CUI, CTransform>() )
    {
        if(ui.get_item(id).follow_cursor){
            auto & transform = transforms.get_item(id);
            glm::vec2 mouse_pos = OpenGlUtil::getMousePos();
            glm::vec2 window_size = OpenGlUtil::getWindowSize();
            float x = (mouse_pos.x/window_size.x) * 2.0f -1.0f;
            float y = -(mouse_pos.y / window_size.y) * 2.0+1.0f;
            transform.position.x = x;
            transform.position.y = y;
        }

    }
}
void SUI::shutdown()
{
}
