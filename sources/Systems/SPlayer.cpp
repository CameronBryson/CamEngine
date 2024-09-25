#include "SPlayer.hpp"
#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "raylib.h"

#include <vector>

static constexpr float forward_movespeed = 1.0f;
static constexpr float movespeed = 10.0f;
static constexpr float rotation_speed = 0.25f;

void s_player::init()
{
    EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyStart, on_input_start_event);
    EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyHold, on_input_hold_event);
    EventHandler::GetInstance()->input_dispatcher.AddListener(InputEvents::KeyEnd, on_input_end_event);
}
void s_player::update(registry & registry, const float dt)
{
    auto player_id = registry.get_entity_ids<c_player>()[0];
    auto& dynamic_body = registry.get_component<c_dynamic_body>(player_id);
    auto& transform = registry.get_component<c_transform>(player_id);

    bool w = engine_util::is_key_pressed('W');
    bool a = engine_util::is_key_pressed('A');
    bool s = engine_util::is_key_pressed('S');
    bool d = engine_util::is_key_pressed('D');
    bool q = engine_util::is_key_pressed('Q');
    bool e = engine_util::is_key_pressed('E');
    bool space = engine_util::is_key_pressed(GLFW_KEY_SPACE);
    bool left_click = engine_util::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT);
    bool right_click = engine_util::is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT);

    if (w) {
        dynamic_body.acceleration.y += movespeed * dt;
        dynamic_body.angular_acceleration.x += rotation_speed * dt;
    }
    if (a) {
        dynamic_body.acceleration.x -= movespeed * dt;
        dynamic_body.angular_acceleration.z -= rotation_speed * dt;
    }
    if (s) {
        dynamic_body.acceleration.y -= movespeed * dt;
        dynamic_body.angular_acceleration.x -= rotation_speed * dt;
    }
    if (d) {
        dynamic_body.acceleration.x += movespeed * dt;
        dynamic_body.angular_acceleration.z += rotation_speed * dt;
    }
    if (q) {
        dynamic_body.acceleration.z += forward_movespeed * dt;
    }
    if (e || space) {
        dynamic_body.acceleration.z -= forward_movespeed * dt;
    }
    if (left_click) {
        printf("left click\n");
        EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateProjectileEvent(registry, transform.position + glm::vec3(0, 0, -5), {0, 0, -1}, 0.1));
    }
    if (right_click) {
        printf("right click\n");
    }

}

void s_player::shutdown()
{
}

void s_player::on_input_start_event(const Event<InputEvents> & event)
{

    auto event_data = event.ToType<KeyPressEvent>();
    if(event_data.key == GLFW_MOUSE_BUTTON_LEFT)
    {
        printf("left click\n");
    }
    else if (event_data.key == 'W')
    {
        printf("W");
    }
    else if (event_data.key == 'A')
    {
        printf("A");
    }
    else if (event_data.key == 'S')
    {
        printf("S");
    }
    else if (event_data.key == 'D')
    {
        printf("D");
    }
    else if (event_data.key == 'Q')
    {
        printf("Q");
    }
    else if (event_data.key == 'E')
    {
        printf("E");
    }
    else if (event_data.key == GLFW_KEY_SPACE)
    {
        printf("SPACE");
    }
}

void s_player::on_input_hold_event(const Event<InputEvents> & event)
{
    auto event_data = event.ToType<KeyHoldEvent>();
}

void s_player::on_input_end_event(const Event<InputEvents> & event)
{
    auto event_data = event.ToType<KeyRelease>();
}
