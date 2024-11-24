#include "InputManager.hpp"
#include "EventHandler.hpp"

InputManager::InputManager()
{
	for (int i = 0; i < 512; ++i)
	{
		mKeyMap[i] = KeyAction::None;
	}
	EventHandler::GetInstance()->inputDispatcher.AddListener(InputEvents::KeyPress, std::bind(&InputManager::onInputPressEvent, this, std::placeholders::_1));
	EventHandler::GetInstance()->inputDispatcher.AddListener(InputEvents::KeyRelease, std::bind(&InputManager::onInputReleaseEvent, this, std::placeholders::_1));
}

InputManager::~InputManager()
{
	
}

KeyAction InputManager::getKeyAction(int key)
{
	return mKeyMap[key];
}

void InputManager::resetKeyStates()
{
	for (auto& [key, action] : mKeyMap)
	{
		if (action == KeyAction::Start)
		{
			action = KeyAction::Hold;
		}
		else if (action == KeyAction::End)
		{
			action = KeyAction::None;
		}
	}
}

void InputManager::onInputPressEvent(const Event<InputEvents>& event)
{
	auto event_data = event.ToType<KeyPressEvent>();
	if (mKeyMap[event_data.key] == KeyAction::None)
	{
		//EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyStartEvent(event_data.key));
		mKeyMap[event_data.key] = KeyAction::Start;
	}
	else
	{
		mKeyMap[event_data.key] = KeyAction::Hold;
	}
}

void InputManager::onInputReleaseEvent(const Event<InputEvents>& event)
{
	auto event_data = event.ToType<KeyReleaseEvent>();
	if (mKeyMap[event_data.key] == KeyAction::Start || mKeyMap[event_data.key] == KeyAction::Hold)
	{
		//EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyEndEvent(event_data.key));
		mKeyMap[event_data.key] = KeyAction::End;
	}
	else
	{
		mKeyMap[event_data.key] = KeyAction::None;
		//this isnt working cause there isnt a new event being sent
	}
}
