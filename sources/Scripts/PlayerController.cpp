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
void PlayerController::Update(float deltaTime)
{
	Camera* m_Camera = &m_Scene->mMainCamera;
	//printf("PlayerController::Update\n");
	auto & dynamic_bodies = m_Scene->getSparseSet<CDynamicBody>();
	auto & players = m_Scene->getSparseSet<Player>();
	auto & transforms = m_Scene->getSparseSet<CTransform>();

	bool w = m_Scene->getKeyAction('W') == KeyAction::Start || m_Scene->getKeyAction('W') == KeyAction::Hold;
	bool a = m_Scene->getKeyAction('A') == KeyAction::Start || m_Scene->getKeyAction('A') == KeyAction::Hold;
	bool s = m_Scene->getKeyAction('S') == KeyAction::Start || m_Scene->getKeyAction('S') == KeyAction::Hold;
	bool d = m_Scene->getKeyAction('D') == KeyAction::Start || m_Scene->getKeyAction('D') == KeyAction::Hold;
	bool left_click = m_Scene->getKeyAction(GLFW_MOUSE_BUTTON_LEFT) == KeyAction::Start;
	bool right_click = engine_util::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

	auto& player = players.get_item(m_OwnerID);
	auto& dynamic_body = dynamic_bodies.get_item(m_OwnerID);

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
		glm::mat4 invVP = glm::inverse(m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() * glm::translate(glm::mat4(1), transforms.get_item(m_OwnerID).position));
		glm::vec4 screenPos = { x, y, 1, 1 };
		glm::vec4 worldPos = (invVP * screenPos);

		glm::vec3 direction = glm::normalize(glm::vec3(worldPos));




		EventHandler::GetInstance()->factoryDispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(m_OwnerID).position, direction, 100.0f, settings::player_bitmask));
	}
}