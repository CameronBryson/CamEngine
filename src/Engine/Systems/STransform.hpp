#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <unordered_set>
#include "Components.hpp"
class BaseScene;
class STransform
{
public:
	STransform(BaseScene* scene);
	void update(float dt);
private:
	BaseScene* mScene;
    // Helper functions
    void updateTransform(entt::entity entity, entt::registry& registry,
        std::unordered_set<entt::entity>& visited,
        const std::function<void(entt::entity)>& updateFunc);

    bool hasPhysicsComponents(entt::entity entity, entt::registry& registry) const;
    glm::mat4 computePhysicsMatrix(entt::entity entity, entt::registry& registry) const;
    glm::mat4 computeLocalMatrix(const CTransform& transform) const;
    glm::mat4 computeModelMatrix(const glm::mat4& parentMatrix, const glm::mat4& physicsMatrix, const glm::mat4& localMatrix, bool hasPhysics) const;
};

