#pragma once
#include "Registry.hpp"

class IScene
{
public:
    virtual ~IScene() = default;
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void lateUpdate(float dt) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual Registry& getRegistry() = 0;
};
