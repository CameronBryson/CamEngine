#pragma once
#include "EventHandler.hpp"
#include "ScriptEvents.hpp"
#include "Registry.hpp"

class ScriptBase
{
  protected:
    ScriptBase(Registry* registry, unsigned owner_ID);
    ~ScriptBase();
  private:
    void BindEvents();
    void UnBindEvents();
  protected:
    Registry* m_Registry;
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
};
