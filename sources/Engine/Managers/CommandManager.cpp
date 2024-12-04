#include "CommandManager.hpp"
#include "Engine/Events/EventHandler.hpp"

CommandManager::CommandManager()
{
	auto* eventHandler = EventHandler::GetInstance();

	// Bind command events and store handles
	mCommandEventHandles.push_back(eventHandler->commandDispatcher.AddListener(
	    CommandEvents::AddCommand, [this](const Event<CommandEvents>& event) { onAddCommandEvent(event); }));

	mCommandEventHandles.push_back(eventHandler->commandDispatcher.AddListener(
	    CommandEvents::ProcessCommands, [this](const Event<CommandEvents>& event) { processCommands(); }));

	mCommandEventHandles.push_back(eventHandler->commandDispatcher.AddListener(
	    CommandEvents::ClearCommands, [this](const Event<CommandEvents>& event) { clearCommands(); }));
}

CommandManager::~CommandManager()
{
	// Unbind event handlers
	auto* eventHandler = EventHandler::GetInstance();

	for (const auto& handle : mCommandEventHandles)
	{
		eventHandler->commandDispatcher.RemoveListener(handle);
	}
	mCommandEventHandles.clear();

	// Clear any remaining commands
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

