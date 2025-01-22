#pragma once
#include <vector>
#include <entt/entity/fwd.hpp>
template<typename T>
class Event;
enum class CollisionEvents;

class BaseScene;

class ScriptBase
{
  protected:
	ScriptBase(BaseScene* scene, entt::entity owner_ID);
	~ScriptBase();
  private:
	void bindEvents();
	void unBindEvents();
  protected:
	BaseScene& GetScene() const { return *mScene; }
	entt::entity GetOwnerID() const { return mOwnerId; }
private:
	BaseScene* mScene;
	entt::entity mOwnerId;
	std::vector<int> mScriptEventHandles;
  private:
	virtual void init()
	{
	}
	virtual void lateInit()
	{
	}
	virtual void update(float deltaTime)
	{
	}
	virtual void lateUpdate(float deltaTime)
	{
	}
	virtual void render()
	{
	}
	virtual void lateRender()
	{
	}
	virtual void shutdown()
	{
	}
	virtual void lateShutdown()
	{
	}
};
