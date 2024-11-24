#pragma once
#include <Engine/Event.hpp>

#include "Engine/IScene.hpp"

#include "Components.hpp"
#include "Engine/CollisionEvents.hpp"
#include "Engine/EventHandler.hpp"
#include "Engine/CommandManager.hpp"
#include "Engine/CollisionManager.hpp"
#include "Engine/InputManager.hpp"
#include "Engine/Factory.hpp"
#include "Engine/Registry.hpp"
#include "Systems/Systems.hpp"
#include "Engine/Timer.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Systems/Systems.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"

class PlayScene final : public IScene
{
public:
    PlayScene();
    ~PlayScene() override;
    void init() override;
    void update(float dt) override;
    void lateUpdate(float dt) override;
    void render() override;
    void shutdown() override;
    Registry& getRegistry() override;

private:
    void initSparseSets();

private:
    Registry mRegistry;
    Factory mFactory;
    Camera mCamera;
    GraphicsManager mGraphicsManager;
    SCollision mCollisionSystem;
    SPhysics mPhysicsSystem;
    SRender mRenderSystem;
	CommandManager mCommandManager;
	CollisionManager mCollisionManager;
	InputManager mInputManager;
};
