#pragma once
class BaseScene;

class SPhysics
{
public:
	SPhysics(BaseScene* scene);
    void init();
    void update(float dt);
    void shutdown();
private:
    void updateDynamicBodies(float dt) const;
	BaseScene* mScene;

};
