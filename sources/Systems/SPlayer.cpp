#include "SPlayer.hpp"
#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "Engine/EventHandler.hpp"
#include "Engine/GameManager.hpp"
#include "Engine/Registry.hpp"
#include "GameSettings.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Graphics/OpenGLUtil.hpp"
#include "glad/gl.h"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <iostream>

#include <GLFW/glfw3.h>
#include <vector>

static constexpr float movespeed = 10.0f;
static constexpr float rotation_speed = 0.25f;

void SPlayer::update(Registry & registry, Camera& camera, const float dt)
{
    auto & dynamic_bodies = registry.getSparseSet<CDynamicBody>();
    auto & players = registry.getSparseSet<CPlayer>();
    auto & transforms = registry.getSparseSet<CTransform>();
    auto ids = registry.getEntityIDs<CPlayer, CDynamicBody, CTransform>();


    bool w = registry.getKeyAction('W') == KeyAction::Start || registry.getKeyAction('W') == KeyAction::Hold;
    bool a = registry.getKeyAction('A') == KeyAction::Start || registry.getKeyAction('A') == KeyAction::Hold;
    bool s = registry.getKeyAction('S') == KeyAction::Start || registry.getKeyAction('S') == KeyAction::Hold;
    bool d = registry.getKeyAction('D') == KeyAction::Start || registry.getKeyAction('D') == KeyAction::Hold;

    bool left_click = registry.getKeyAction(GLFW_MOUSE_BUTTON_LEFT) == KeyAction::Start;
    bool right_click = engine_util::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

    for( const auto id : ids )
    {

        //maybe add some kind of cap so that the ship doesnt rotate beyond 90 degrees
        auto & player = players.get_item(id);
        auto & dynamic_body = dynamic_bodies.get_item(id);
        //velocity = { 0, 0, 0 };
        if( w ){
            dynamic_body.acceleration.y += movespeed;
            dynamic_body.angular_acceleration.x += rotation_speed;
        }
        if( a ){
            dynamic_body.acceleration.x -= movespeed;
            dynamic_body.angular_acceleration.z -= rotation_speed;
        }
        if( s ) {
            dynamic_body.acceleration.y -= movespeed;
            dynamic_body.angular_acceleration.x -= rotation_speed;
        }
        if( d ) {
            dynamic_body.acceleration.x += movespeed;
            dynamic_body.angular_acceleration.z += rotation_speed;
        }
        if(left_click)
        {

            //printf("left click\n");
            glm::vec2 mouse_pos = OpenGlUtil::getMousePos();

            glm::vec2 window_size = OpenGlUtil::getWindowSize();
            float x = (mouse_pos.x/window_size.x) * 2.0f -1.0f;
            float y = -(mouse_pos.y / window_size.y) * 2.0+1.0f;
            //std::cout<< x << " " << y << std::endl;
            //ray cast mouse to 3d space
            glm::mat4 invVP = glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix() *glm::translate(glm::mat4(1),transforms.get_item(id).position ));
            glm::vec4 screenPos = {x,y,1,1};
            glm::vec4 worldPos = (invVP * screenPos);
             //worldPos.w = 1.0f/worldPos.w;
             //worldPos.x *= worldPos.w;
             //worldPos.y *= worldPos.w;
             //worldPos.z *= worldPos.w;

            glm::vec3 direction = glm::normalize(glm::vec3(worldPos));

            //std::cout << direction.x << " " << direction.y << " " << direction.z  <<  " " << std::endl;

            

            EventHandler::GetInstance()->factoryDispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(id).position, direction, 100.0f,settings::player_bitmask));

//            EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(id).position+glm::vec3(0,5,-5), {0,0,-1}, 0.1));
        }
        if(right_click){
            printf("right click\n");
        }

    }
}

void SPlayer::shutdown()
{
}

