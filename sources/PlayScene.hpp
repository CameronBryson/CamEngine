#pragma once
#include "IScene.hpp"
#include "Registry.hpp"
#include "raylib.h"
#include "Systems.hpp"
class PlayScene final : public IScene{
public:
    PlayScene();
    ~PlayScene() override;
    void Init() override;
    void Update(float dt) override;
    void Render() override;
    void Shutdown() override;
    Registry& GetRegistry() override;
private:
    void InitSparseSets();
private:
    Registry m_Registry;
    Camera3D m_Camera;

};

