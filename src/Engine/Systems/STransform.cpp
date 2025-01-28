#include "pch.hpp"
#include "BaseScene.hpp"
#include "STransform.hpp"
#include "Components.hpp" // Include Components.hpp directly

#include <edyn/comp/present_orientation.hpp>
#include <edyn/comp/present_position.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <entt/entity/utility.hpp>

STransform::STransform(BaseScene* scene)
    : mScene(scene) {
}

void STransform::update(float dt) {
    auto& registry = mScene->mEnttRegistry;

    auto view = registry.view<CTransform>(entt::exclude<CParent>);

    for (auto entity : view) {
        updateTransform(entity, registry);
    }
}

void STransform::updateTransform(entt::entity entity, entt::registry& registry) {
    auto& transform = registry.get<CTransform>(entity);

    glm::mat4 localMatrix = computeLocalMatrix(transform);

    glm::mat4 physicsMatrix = glm::mat4(1.0f);
    bool hasPhysics = hasPhysicsComponents(entity, registry);
    if (hasPhysics) {
        physicsMatrix = computePhysicsMatrix(entity, registry);
    }

    if (registry.any_of<CParent>(entity)) {
        auto parent = registry.get<CParent>(entity).parent;
        const auto& parentTransform = registry.get<CTransform>(parent);
        transform.model_matrix = parentTransform.model_matrix * localMatrix * physicsMatrix;
    }
    else {
        transform.model_matrix = localMatrix * physicsMatrix;
    }

    if (registry.any_of<CChildren>(entity)) {
        auto& children = registry.get<CChildren>(entity).children;
        for (auto child : children) {
            updateTransform(child, registry);
        }
    }
}

glm::mat4 STransform::computeLocalMatrix(const CTransform& transform) const {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.position);
    glm::mat4 rotationMatrix = glm::mat4_cast(transform.rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

bool STransform::hasPhysicsComponents(entt::entity entity, entt::registry& registry) const {
    return registry.all_of<edyn::present_position, edyn::present_orientation>(entity);
}

glm::mat4 STransform::computePhysicsMatrix(entt::entity entity, entt::registry& registry) const {
    const auto& physicsPos = registry.get<edyn::present_position>(entity);
    const auto& physicsOri = registry.get<edyn::present_orientation>(entity);

    glm::vec3 position(physicsPos.x, physicsPos.y, physicsPos.z);
    glm::quat orientation(physicsOri.w, physicsOri.x, physicsOri.y, physicsOri.z);
    orientation = glm::normalize(orientation);

    return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(orientation);
}
