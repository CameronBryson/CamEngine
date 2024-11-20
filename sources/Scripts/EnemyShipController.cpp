#include "EnemyShipController.hpp"
#include "Scripts/EnemyShip.hpp"
void EnemyShipController::Update(float deltaTime)
{
    auto & enemies = m_Registry->getSparseSet<EnemyShip>();
    auto & transforms = m_Registry->getSparseSet<CTransform>();
    auto & dynamic_bodies = m_Registry->getSparseSet<CDynamicBody>();
    auto ids = m_Registry->getEntityIDs<EnemyShip, CTransform, CDynamicBody>();
    for( unsigned short id : ids )
    {
	auto & enemy = enemies.get_item(id);
	auto & enemy_tranform = transforms.get_item(id);
	auto & enemy_dynamic_body = dynamic_bodies.get_item(id);
	auto & target_transform = transforms.get_item(enemy.target);
	enemy.timeSinceShoot += deltaTime;
	if( enemy.timeSinceShoot > enemy.shootCooldown )
	{
	    //shoot
	    //printf("Shoot\n");
	    glm::vec3 direction = glm::normalize(target_transform.position - enemy_tranform.position);


	    //EventHandler::GetInstance()->factoryDispatcher.SendEvent(
		//CreateProjectileEvent(transforms.get_item(id).position, direction, 50.0f, settings::enemy_bitmask));

	    enemy.timeSinceShoot = 0.0f;
	}
	enemy_dynamic_body.acceleration += enemy.direction * enemy.speed;
    }
}
