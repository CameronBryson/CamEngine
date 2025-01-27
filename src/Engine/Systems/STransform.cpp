#include "pch.hpp"
#include "STransform.hpp"
#include "BaseScene.hpp"
#include <edyn/comp/present_position.hpp>
#include "Components.hpp"
#include <edyn/comp/present_orientation.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <iostream>



STransform::STransform(BaseScene* scene) : mScene(scene)
{
}

bool STransform::hasPhysicsComponents(entt::entity entity, entt::registry& registry) const {
    return registry.all_of<edyn::present_position, edyn::present_orientation>(entity);
}

glm::mat4 STransform::computePhysicsMatrix(entt::entity entity, entt::registry& registry) const {
    const auto& physicsPos = registry.get<edyn::present_position>(entity);
    const auto& physicsOri = registry.get<edyn::present_orientation>(entity);

    glm::vec3 globalPosition(physicsPos.x, physicsPos.y, physicsPos.z);
    glm::quat globalOrientation(physicsOri.w, physicsOri.x, physicsOri.y, physicsOri.z);
    globalOrientation = glm::normalize(globalOrientation);

    return glm::translate(glm::mat4(1.0f), globalPosition) * glm::mat4_cast(globalOrientation);
}

glm::mat4 STransform::computeLocalMatrix(const CTransform& transform) const {
    glm::mat4 local = glm::translate(glm::mat4(1.0f), transform.position);
    local *= glm::mat4_cast(transform.rotation); 
    local = glm::scale(local, transform.scale);
    return local;
}

glm::mat4 STransform::computeModelMatrix(const glm::mat4& parentMatrix, const glm::mat4& physicsMatrix, const glm::mat4& localMatrix, bool hasPhysics) const {
    if (hasPhysics) {
        // Parent * Physics * Local
        return parentMatrix * physicsMatrix * localMatrix;
    }
    else {
        // Parent * Local
        return parentMatrix * localMatrix;
    }
}

void STransform::updateTransform(entt::entity entity, entt::registry& registry,
    std::unordered_set<entt::entity>& visited,
    const std::function<void(entt::entity)>& updateFunc)
{

    visited.insert(entity);

    auto& transform = registry.get<CTransform>(entity);

    glm::mat4 physicsMatrix(1.0f);
    bool hasPhysics = false;

    if (hasPhysicsComponents(entity, registry)) {
        hasPhysics = true;
        physicsMatrix = computePhysicsMatrix(entity, registry);
    }

    glm::mat4 parentMatrix(1.0f);

    if (transform.parent != entt::null && registry.valid(transform.parent)) {
        auto& parentTransform = registry.get<CTransform>(transform.parent);

        if (parentTransform.dirty) {
            updateTransform(transform.parent, registry, visited, updateFunc);
        }

        parentMatrix = parentTransform.model_matrix;
    }

    glm::mat4 localMatrix = computeLocalMatrix(transform);

    glm::mat4 modelMatrix = computeModelMatrix(parentMatrix, physicsMatrix, localMatrix, hasPhysics);

    transform.model_matrix = modelMatrix;
    transform.dirty = false;

    visited.erase(entity);
}

void STransform::update(float dt) {
    auto& registry = mScene->mEnttRegistry;

    // To prevent cyclic dependencies
    std::unordered_set<entt::entity> visited;

    std::function<void(entt::entity)> updateFunc = [&](entt::entity entity) {
        updateTransform(entity, registry, visited, updateFunc);
        };

    auto view = registry.view<CTransform>();

    for (auto entity : view) {
        auto& transform = view.get<CTransform>(entity);
            updateFunc(entity);
        if (transform.dirty) {
        }
    }
}

