#ifndef ISCENE_HPP
#define ISCENE_HPP

#include "Registry.hpp"
class IScene{
public:
    virtual ~IScene() = default;
    virtual void Init()=0;
    virtual void Update(float dt)=0;
    virtual void Render() = 0;
    virtual void Shutdown()=0;
    virtual Registry* GetRegistry() = 0;
};

#endif // ISCENE_HPP
