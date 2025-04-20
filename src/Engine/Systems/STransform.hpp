#pragma once

#include <glm/mat4x4.hpp>
#include <entt/entity/registry.hpp>
class BaseScene;
class STransform
{
public:
	explicit STransform(BaseScene* scene);
	void update(float dt) const;

private:
	BaseScene* mScene;

	static void updateTransform(entt::entity entity, entt::registry& registry);
	static glm::mat4 computeLocalMatrix(const CTransform& transform);
};
