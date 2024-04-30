#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP

#include "IScene.hpp"
#include "Registry.hpp"
#include "SPhysics.hpp"
class PlayScene: public IScene{
public:
    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Shutdown() override;
    Registry& GetRegistry() override;
private:
    Registry m_Registry;

    //Systems
    SPhysics physicsSystem;

};

#endif // PLAYSCENE_HPP
