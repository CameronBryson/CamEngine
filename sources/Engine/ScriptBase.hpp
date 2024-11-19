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
    virtual void Init() = 0;
    virtual void LateInit() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void LateUpdate(float deltaTime) = 0;
    virtual void Shutdown() = 0;
};
