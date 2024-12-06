#include "Engine/pch.hpp"
#include "TestSceneController.hpp"

#include <glm/gtc/matrix_transform.hpp>

TestSceneController::TestSceneController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
{
	printf("TestSceneController created\n");
}

void TestSceneController::init() { m_Camera = &GetScene().mMainCamera; }

void TestSceneController::update(float deltaTime) {}
