#pragma once
#include "Engine/Storage/Registry.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Managers/CollisionManager.hpp"
#include "Engine/Managers/CommandManager.hpp"
#include "Engine/Managers/InputManager.hpp"
#include "Engine/Managers/AudioManager.hpp"

#include "Engine/Systems/SCollision.hpp"
#include "Engine/Systems/SPhysics.hpp"
#include "Engine/Systems/SRender.hpp"
#include "Engine/Systems/SAudio.hpp"

#include "Engine/Graphics/Camera.hpp"

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
	virtual void shutdown();

	unsigned short createEntity();

	void deleteEntity(unsigned short id);

	template <typename T, typename... Args>
	void addComponent(unsigned short id, Args&&... componentArgs);

	template <typename T>
	T& getComponent(unsigned short id);

	template <typename T>
	bool hasComponent(unsigned short id);

	template <typename T>
	void removeComponent(unsigned short id);

	template <typename... T>
	std::vector<unsigned short> getEntityIDs();
	template<typename T>
	SparseSet<T>& getSparseSet();

	template<typename T>
	bool hasSparseSet() const;

	KeyAction getKeyAction(int key);

private:
	void initSpaseSets();

public:
	Registry mRegistry;

	//hold data
	GraphicsManager mGraphicsManager;
	CommandManager mCommandManager;
	CollisionManager mCollisionManager;
	InputManager mInputManager;
	AudioManager mAudioManager;

	//dont hold data or very little data
	SCollision mCollisionSystem;
	SPhysics mPhysicsSystem;
	SRender mRenderSystem;
	SAudio mAudioSystem;

	Camera mMainCamera;
	//current snapshot?


};

template<typename T, typename ...Args>
inline void BaseScene::addComponent(unsigned short id, Args && ...componentArgs)
{
	mRegistry.addComponent<T>(id, std::forward<Args>(componentArgs)...);
}

template<typename T>
inline T& BaseScene::getComponent(unsigned short id)
{
	return mRegistry.getComponent<T>(id);
}

template<typename T>
inline bool BaseScene::hasComponent(unsigned short id)
{
	return mRegistry.hasComponent<T>(id);
}

template<typename T>
inline void BaseScene::removeComponent(unsigned short id)
{
	mRegistry.removeComponent<T>(id);
}

template<typename ...T>
inline std::vector<unsigned short> BaseScene::getEntityIDs()
{
	return mRegistry.getEntityIDs<T...>();
}

template<typename T>
inline SparseSet<T>& BaseScene::getSparseSet()
{
	return mRegistry.getSparseSet<T>();
}

template<typename T>
inline bool BaseScene::hasSparseSet() const
{
	return mRegistry.hasSparseSet<T>();
}
