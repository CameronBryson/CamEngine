#include "pch.hpp"
#include "BaseScene.hpp"
#include "STransform.hpp"
#include "Components.hpp" // Include Components.hpp directly

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

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


    if (registry.any_of<CParent>(entity)) {
        auto parent = registry.get<CParent>(entity).parent;
        const auto& parentTransform = registry.get<CTransform>(parent);
        transform.model_matrix = parentTransform.model_matrix * localMatrix;
    }
    else {
        transform.model_matrix = localMatrix;
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
