#pragma once
template<typename T>
class Event;
enum class CollisionEvents;

class BaseScene;

class ScriptBase
{
  protected:
	ScriptBase(BaseScene* scene, unsigned short owner_ID);
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
	virtual void onCollisionEnter(unsigned short firstID, unsigned short secondID)
	{
	}
	virtual void onCollisionStay(unsigned short firstID, unsigned short secondID)
	{
	}
	virtual void onCollisionExit(unsigned short firstID, unsigned short secondID)
	{
	}
};
