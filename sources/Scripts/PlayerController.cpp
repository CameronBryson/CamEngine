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
	//printf("PlayerController::Update\n");
	auto & dynamic_bodies = m_Registry->getSparseSet<CDynamicBody>();
	auto & players = m_Registry->getSparseSet<Player>();
	auto & transforms = m_Registry->getSparseSet<CTransform>();

	/*bool w = m_Registry->getKeyAction('W') == KeyAction::Start || m_Registry->getKeyAction('W') == KeyAction::Hold;
	bool a = m_Registry->getKeyAction('A') == KeyAction::Start || m_Registry->getKeyAction('A') == KeyAction::Hold;
	bool s = m_Registry->getKeyAction('S') == KeyAction::Start || m_Registry->getKeyAction('S') == KeyAction::Hold;
	bool d = m_Registry->getKeyAction('D') == KeyAction::Start || m_Registry->getKeyAction('D') == KeyAction::Hold;*/
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;

	bool left_click= false;
	//bool left_click = m_Registry->getKeyAction(GLFW_MOUSE_BUTTON_LEFT) == KeyAction::Start;
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