#pragma once
class BaseScene;
class SAudio
{
public:
	SAudio(BaseScene* scene);
	void init();
	void update(float deltaTime);
	void shutdown();
private:
	BaseScene* mScene;
};