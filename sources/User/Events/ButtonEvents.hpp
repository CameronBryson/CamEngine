#pragma once
#include "Engine/Events/Event.hpp"
enum class 
{
	StartButton,
	QuitButton
};
class StartButtonEvent final : public Event<ButtonEvents>
{
public:
	explicit StartButtonEvent() : Event<ButtonEvents>(ButtonEvents::StartButton, "StartButton") {}
};
class QuitButtonEvent final : public Event<ButtonEvents>
{
public:
	explicit QuitButtonEvent() : Event<ButtonEvents>(ButtonEvents::QuitButton, "QuitButton") {}
}