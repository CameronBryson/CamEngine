#pragma once
#include <Registry.hpp>
#include <Components.hpp>


class Factory {
public:
    static unsigned short CreatePlayer(Registry& registry) {
        auto ID = registry.createEntity();
        registry.addComponent<CPlayer>(ID, CPlayer());
        registry.addComponent<CTransform>(ID, CTransform());
        registry.addComponent<CVelocity>(ID, CVelocity());
        registry.addComponent<CRigidBody>(ID, CRigidBody{.drag = 0.9f});
        registry.addComponent<CAABB>(ID, CAABB{.extents = {1.0f, 1.0f, 1.0f}});
        return ID;
    }
};