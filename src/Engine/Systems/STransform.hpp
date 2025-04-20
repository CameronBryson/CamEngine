#pragma once

#include <glm/mat4x4.hpp>
#include <entt/entity/registry.hpp>
class BaseScene;
class STransform
{
public:
	STransform(BaseScene* scene);
	void update(float dt);

private:
	BaseScene* mScene;

	void updateTransform(entt::entity entity, entt::registry& registry);
	glm::mat4 computeLocalMatrix(const CTransform& transform) const;
};
