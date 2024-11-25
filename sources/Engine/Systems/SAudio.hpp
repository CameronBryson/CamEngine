#pragma once
class BaseScene;
class SAudio
{
public:
	SAudio(BaseScene*);
	void init();
	void update(float deltaTime);
	void shutdown();
private:
	BaseScene* mScene;
};