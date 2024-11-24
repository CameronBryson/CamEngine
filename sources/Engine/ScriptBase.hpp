#pragma once
#include "EventHandler.hpp"
#include "ScriptEvents.hpp"
#include "BaseScene.hpp"

class ScriptBase
{
  protected:
	ScriptBase(BaseScene* scene, unsigned owner_ID);
	~ScriptBase();
  private:
	void BindEvents();
	void UnBindEvents();
	void OnCollisionEnterEvent(const Event<CollisionEvents>& event);
	void OnCollisionStayEvent(const Event<CollisionEvents>& event);
	void OnCollisionExitEvent(const Event<CollisionEvents>& event);
  protected:
	BaseScene* m_Scene;
	unsigned m_OwnerID;
  private:
	virtual void Init()
	{
	}
	virtual void LateInit()
	{
	}
	virtual void Update(float deltaTime)
	{
	}
	virtual void LateUpdate(float deltaTime)
	{
	}
	virtual void Shutdown()
	{
	}
	virtual void OnCollisionEnter(unsigned otherID)
	{
	}
	virtual void OnCollisionStay(unsigned otherID)
	{
	}
	virtual void OnCollisionExit(unsigned otherID)
	{
	}
};
