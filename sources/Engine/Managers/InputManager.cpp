#include "InputManager.hpp"
#include "Engine/Events/EventHandler.hpp"

InputManager::InputManager()
{
	for (int i = 0; i < 512; ++i)
	{
		mKeyMap[i] = KeyAction::None;
	}

	auto* eventHandler = EventHandler::GetInstance();

	// Bind input events and store handles
	mInputEventHandles.push_back(eventHandler->inputDispatcher.AddListener(
	    InputEvents::KeyPress, [this](const Event<InputEvents>& event) { onInputPressEvent(event); }));

	mInputEventHandles.push_back(eventHandler->inputDispatcher.AddListener(
	    InputEvents::KeyRelease, [this](const Event<InputEvents>& event) { onInputReleaseEvent(event); }));
}

InputManager::~InputManager()
{
	auto* eventHandler = EventHandler::GetInstance();

	// Unbind input events using stored handles
	for (const auto& handle : mInputEventHandles)
	{
		eventHandler->inputDispatcher.RemoveListener(handle);
	}
	mInputEventHandles.clear();
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
	const auto& eventData = event.ToType<KeyPressEvent>();
	if (mKeyMap[eventData.key] == KeyAction::None)
	{
		//EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyStartEvent(event_data.key));
		mKeyMap[eventData.key] = KeyAction::Start;
	}
	else
	{
		mKeyMap[eventData.key] = KeyAction::Hold;
	}
}

void InputManager::onInputReleaseEvent(const Event<InputEvents>& event)
{
	const auto& eventData = event.ToType<KeyReleaseEvent>();
	if (mKeyMap[eventData.key] == KeyAction::Start || mKeyMap[eventData.key] == KeyAction::Hold)
	{
		//EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyEndEvent(event_data.key));
		mKeyMap[eventData.key] = KeyAction::End;
	}
	else
	{
		mKeyMap[eventData.key] = KeyAction::None;
		//this isnt working cause there isnt a new event being sent
	}
}
