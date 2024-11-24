#include "CommandManager.hpp"
#include "EventHandler.hpp"
#include "Registry.hpp"

CommandManager::CommandManager()
{
	EventHandler::GetInstance()->commandDispatcher.AddListener(CommandEvents::AddCommand, std::bind(&CommandManager::onAddCommandEvent, this, std::placeholders::_1));
	EventHandler::GetInstance()->commandDispatcher.AddListener(CommandEvents::ProcessCommands, std::bind(&CommandManager::processCommands, this));
	EventHandler::GetInstance()->commandDispatcher.AddListener(CommandEvents::ClearCommands, std::bind(&CommandManager::clearCommands, this));
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
	const auto eventData = event.ToType<AddCommandEvent>();
	addCommand(eventData.mCommand);
}

