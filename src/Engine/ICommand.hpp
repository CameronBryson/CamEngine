#pragma once
class Registry;
class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
};