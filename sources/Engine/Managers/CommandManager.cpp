#include "CommandManager.hpp"
#include "Engine/Events/EventHandler.hpp"

CommandManager::CommandManager()
{
	EventHandler::GetInstance()->commandDispatcher.AddListener(CommandEvents::AddCommand, [this](const Event<CommandEvents>& event)
		{
			onAddCommandEvent(event);
		});
	EventHandler::GetInstance()->commandDispatcher.AddListener(CommandEvents::ProcessCommands, [this](const Event<CommandEvents>& event)
		{
			processCommands();
		});
	EventHandler::GetInstance()->commandDispatcher.AddListener(CommandEvents::ClearCommands, [this](const Event<CommandEvents>& event)
		{
			clearCommands();
		});
}

CommandManager::~CommandManager()
{
	clearCommands();
}

void CommandManager::addCommand(std::shared_ptr<ICommand> command) {
	mCommandQueue.push(std::move(command));
}
void CommandManager::processCommands() {
	while (!mCommandQueue.empty()) {
		const auto& command = mCommandQueue.front();
		command->execute();
		mCommandQueue.pop();
	}
}

void CommandManager::clearCommands()
{
	while (!mCommandQueue.empty())
	{
		mCommandQueue.pop();
	}
}

void CommandManager::onAddCommandEvent(const Event<CommandEvents>& event)
{
	const auto& eventData = event.ToType<AddCommandEvent>();
	addCommand(eventData.mCommand);
}

