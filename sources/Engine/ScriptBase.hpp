#pragma once
#include "EventHandler.hpp"
#include "ScriptEvents.hpp"
class BaseScene;

class ScriptBase
{
  protected:
	ScriptBase(BaseScene* scene, unsigned owner_ID);
	~ScriptBase();
  private:
	void bindEvents();
	void unBindEvents();
	void onCollisionEnterEvent(const Event<CollisionEvents>& event);
	void onCollisionStayEvent(const Event<CollisionEvents>& event);
	void onCollisionExitEvent(const Event<CollisionEvents>& event);
  protected:
	BaseScene& GetScene() const { return *mScene; }
	unsigned short GetOwnerID() const { return mOwnerId; }
private:
	BaseScene* mScene;
	unsigned short mOwnerId;
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
	virtual void shutdown()
	{
	}
	virtual void onCollisionEnter(unsigned otherID)
	{
	}
	virtual void onCollisionStay(unsigned otherID)
	{
	}
	virtual void onCollisionExit(unsigned otherID)
	{
	}
};
