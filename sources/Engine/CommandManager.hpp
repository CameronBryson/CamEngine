#pragma once
#include <memory>
#include <queue>
#include "CommandEvents.hpp"
class Registry;

class CommandManager {
public:
    CommandManager();
    ~CommandManager();
    void addCommand(std::shared_ptr<ICommand> command);
    void processCommands();
	void clearCommands();

	void onAddCommandEvent(const Event<CommandEvents>& event);

private:
    std::queue<std::shared_ptr<ICommand>> mCommandQueue;
};