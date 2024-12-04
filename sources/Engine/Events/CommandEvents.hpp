#pragma once
#include "Engine/Events/Event.hpp"
#include "Engine/ICommand.hpp"
#include <memory>
enum class CommandEvents
{
	AddCommand,
	ProcessCommands,
	ClearCommands,
	ChangeScene,

};
class AddCommandEvent final : public Event<CommandEvents>
{
public:
	explicit AddCommandEvent(std::shared_ptr<ICommand> command) : Event<CommandEvents>(CommandEvents::AddCommand, "AddCommand"), mCommand(std::move(command)) {}
	std::shared_ptr<ICommand> mCommand;
};
class ProcessCommandEvent final : public Event<CommandEvents>
{
public:
	explicit ProcessCommandEvent() : Event<CommandEvents>(CommandEvents::ProcessCommands, "ProcessCommand") {}
};
