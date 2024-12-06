#include "Engine/pch.hpp"
#include "EnemyShipController.hpp"
#include "User/Scripts/Components/EnemyShip.hpp"
#include "Engine/Base/BaseScene.hpp"
void EnemyShipController::update(float deltaTime)
{
    auto & enemies = GetScene().getSparseSet<EnemyShip>();
    auto & transforms = GetScene().getSparseSet<CTransform>();
    auto & dynamic_bodies = GetScene().getSparseSet<CDynamicBody>();
    auto ids = GetScene().getEntityIDs<EnemyShip, CTransform, CDynamicBody>();
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
