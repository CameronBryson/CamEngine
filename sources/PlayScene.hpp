#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include "IScene.hpp"
#include "Registry.hpp"
#include "SPhysics.hpp"

#include <SPlayer.hpp>
#include <SRender.hpp>
class PlayScene final : public IScene{
public:
    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Shutdown() override;
    Registry& GetRegistry() override;
private:
    void InitSparseSets();
private:
    Registry m_Registry;
    //Systems
    SPhysics m_physicsSystem;
    SPlayer m_playerSystem;
    SRender m_renderSystem;

};

#endif // PLAYSCENE_HPP
