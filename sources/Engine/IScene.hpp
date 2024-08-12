#pragma once
#include "Registry.hpp"

class i_scene
{
public:
    virtual ~i_scene() = default;
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void late_update(float dt) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual registry& get_registry() = 0;
};
