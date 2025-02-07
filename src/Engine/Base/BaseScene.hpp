#pragma once

#include "Engine/Systems/SAudio.hpp"
#include "Engine/Systems/SRender.hpp"
#include "Engine/Systems/STransform.hpp"

#include "Engine/Graphics/Camera.hpp"
#include <entt/entt.hpp>
#include "EnvironmentMap.hpp"

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
	//AudioManager mAudioManager;

	//dont hold data or very little data
	SRender mRenderSystem;
	SAudio mAudioSystem;
	STransform mTransformSystem;

	Camera mCurrentCamera;
	EnvironmentMap mEnvironmentMap;
	//current snapshot?


};
