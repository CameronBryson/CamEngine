#pragma once
#include "Engine/Managers/CommandManager.hpp"
#include "Engine/Managers/InputManager.hpp"

#include "Engine/Systems/SRender.hpp"
#include "Engine/Systems/SAudio.hpp"

#include "Engine/Graphics/Camera.hpp"
#include <entt/entt.hpp>
#include <KeyAction.hpp>

class BaseScene
{
public:
	BaseScene();
	virtual ~BaseScene() = default;
	virtual void init();
	virtual void lateInit();
	virtual void update(float dt);
	virtual void lateUpdate(float dt);
	virtual void render();
	virtual void lateRender();
	virtual void shutdown();
	virtual void lateShutdown();



public:
	entt::registry mEnttRegistry;


	//hold data
	//GraphicsManager mGraphicsManager;
	CommandManager mCommandManager;
	InputManager mInputManager;
	//AudioManager mAudioManager;

	//dont hold data or very little data
	SRender mRenderSystem;
	SAudio mAudioSystem;

	Camera mMainCamera;
	//current snapshot?


};
