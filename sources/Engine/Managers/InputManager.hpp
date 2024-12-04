#pragma once
#include "Engine/Events/InputEvents.hpp"
#include "Engine/KeyAction.hpp"
#include <unordered_map>

class InputManager
{
public:
    InputManager();
    ~InputManager();
    KeyAction getKeyAction(int key);
    void resetKeyStates();
private:
	void onInputPressEvent(const Event<InputEvents>& event);
	void onInputReleaseEvent(const Event<InputEvents>& event);
	std::unordered_map<int, KeyAction> mKeyMap;
	std::vector<int> mInputEventHandles;
};