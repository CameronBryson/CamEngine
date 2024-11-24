#include "PlayerController.hpp"
#include "Engine/EngineUtil.hpp"
#include "Graphics/OpenGLUtil.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "Scripts/Player.hpp"
#include "Engine/BaseScene.hpp"
void PlayerController::update(float deltaTime)
{
	
	Camera* m_Camera = &GetScene().mMainCamera;
	//printf("PlayerController::Update\n");
	auto & dynamic_bodies = GetScene().getSparseSet<CDynamicBody>();
	auto & players = GetScene().getSparseSet<Player>();
	auto & transforms = GetScene().getSparseSet<CTransform>();

	bool w = GetScene().getKeyAction('W') == KeyAction::Start || GetScene().getKeyAction('W') == KeyAction::Hold;
	bool a = GetScene().getKeyAction('A') == KeyAction::Start || GetScene().getKeyAction('A') == KeyAction::Hold;
	bool s = GetScene().getKeyAction('S') == KeyAction::Start || GetScene().getKeyAction('S') == KeyAction::Hold;
	bool d = GetScene().getKeyAction('D') == KeyAction::Start || GetScene().getKeyAction('D') == KeyAction::Hold;
	bool left_click = GetScene().getKeyAction(GLFW_MOUSE_BUTTON_LEFT) == KeyAction::Start;
	bool right_click = engine_util::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

	auto& player = players.get_item(GetOwnerID());
	auto& dynamic_body = dynamic_bodies.get_item(GetOwnerID());

	if( w )
	{
		dynamic_body.acceleration.y += movespeed;
		dynamic_body.angular_acceleration.x += rotation_speed;
	}
	if( a )
	{
		dynamic_body.acceleration.x -= movespeed;
		dynamic_body.angular_acceleration.z -= rotation_speed;
	}
	if( s )
	{
		dynamic_body.acceleration.y -= movespeed;
		dynamic_body.angular_acceleration.x -= rotation_speed;
	}
	if( d )
	{
		dynamic_body.acceleration.x += movespeed;
		dynamic_body.angular_acceleration.z += rotation_speed;
	}
	if( left_click )
	{
		glm::vec2 mouse_pos = OpenGlUtil::getMousePos();

		glm::vec2 window_size = OpenGlUtil::getWindowSize();
		float x = (mouse_pos.x / window_size.x) * 2.0f - 1.0f;
		float y = -(mouse_pos.y / window_size.y) * 2.0 + 1.0f;
		glm::mat4 invVP = glm::inverse(m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * glm::translate(glm::mat4(1), transforms.get_item(GetOwnerID()).position));
		glm::vec4 screenPos = { x, y, 1, 1 };
		glm::vec4 worldPos = (invVP * screenPos);

		glm::vec3 direction = glm::normalize(glm::vec3(worldPos));




		EventHandler::GetInstance()->factoryDispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(GetOwnerID()).position, direction, 100.0f, settings::player_bitmask));
	}
}