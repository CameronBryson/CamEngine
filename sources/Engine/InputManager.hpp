#pragma once
#include "InputEvents.hpp"
#include "KeyAction.hpp"
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
};