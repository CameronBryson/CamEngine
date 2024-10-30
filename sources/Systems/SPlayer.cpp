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

#include <GLFW/glfw3.h>
#include <vector>

static constexpr float movespeed = 10.0f;
static constexpr float rotation_speed = 0.25f;

void s_player::update(registry & registry, Camera& camera, const float dt)
{
    auto & dynamic_bodies = registry.get_sparse_set<c_dynamic_body>();
    auto & players = registry.get_sparse_set<c_player>();
    auto & transforms = registry.get_sparse_set<c_transform>();
    auto ids = registry.get_entity_ids<c_player, c_dynamic_body, c_transform>();


    bool w = registry.get_key_action('W') == KeyAction::Start || registry.get_key_action('W') == KeyAction::Hold;
    bool a = registry.get_key_action('A') == KeyAction::Start || registry.get_key_action('A') == KeyAction::Hold;
    bool s = registry.get_key_action('S') == KeyAction::Start || registry.get_key_action('S') == KeyAction::Hold;
    bool d = registry.get_key_action('D') == KeyAction::Start || registry.get_key_action('D') == KeyAction::Hold;

    bool left_click = registry.get_key_action(GLFW_MOUSE_BUTTON_LEFT) == KeyAction::Start;
    bool right_click = engine_util::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT);

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
            glm::vec2 mouse_pos = opengl_util::get_mouse_pos();

            glm::vec2 window_size = opengl_util::get_window_size();
            float x = (mouse_pos.x/window_size.x) * 2.0f -1.0f;
            float y = -(mouse_pos.y / window_size.y) * 2.0+1.0f;
            std::cout<< x << " " << y << std::endl;
            //ray cast mouse to 3d space
            glm::mat4 invVP = glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix() *glm::translate(glm::mat4(1),transforms.get_item(id).position ));
            glm::vec4 screenPos = {x,y,1,1};
            glm::vec4 worldPos = (invVP * screenPos);
             //worldPos.w = 1.0f/worldPos.w;
             //worldPos.x *= worldPos.w;
             //worldPos.y *= worldPos.w;
             //worldPos.z *= worldPos.w;

            glm::vec3 direction = glm::normalize(glm::vec3(worldPos));

            std::cout << direction.x << " " << direction.y << " " << direction.z  <<  " " << std::endl;

            

            EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(id).position, direction, 100.0f,settings::player_bitmask));

//            EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(id).position+glm::vec3(0,5,-5), {0,0,-1}, 0.1));
        }
        if(right_click){
            printf("right click\n");
        }

    }
}

void s_player::shutdown()
{
}

