#include "MainMenuController.hpp"
#include <Engine/KeyAction.hpp>
#include "Engine/Base/BaseScene.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "User/Scenes/PlayScene.hpp"
void MainMenuController::update(float deltaTime)
{ 
	bool space = GetScene().getKeyAction(' ') == KeyAction::Start || GetScene().getKeyAction(' ') == KeyAction::Hold;
	if (space)
	{
		printf("Space Pressed\n");
		GameManager::loadScene<PlayScene>();
	}
}