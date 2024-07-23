//
// Created by cam on 23/07/24.
//

#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <Registry.hpp>
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
};
template<typename T>
class AddComponentCommand : public ICommand {
public:
    AddComponentCommand(SparseSet<T>& sparseSet, unsigned short ID, T componentData) : m_SparseSet(sparseSet), m_ID(ID), m_ComponentData(std::move(componentData)) {}
    void Execute() override {
        m_SparseSet.addItem(m_ID, m_ComponentData);
    }
private:
    SparseSet<T>& m_SparseSet;
    unsigned short m_ID;
    T m_ComponentData;
};

template<typename T>
class RemoveComponentCommand : public ICommand {
public:
    RemoveComponentCommand(SparseSet<T>& sparseSet, unsigned short ID) : m_SparseSet(sparseSet), m_ID(ID) {}
    void Execute() override {
        m_SparseSet.removeItem(m_ID);
    }
private:
    SparseSet<T>& m_SparseSet;
    unsigned short m_ID;
};
#endif //COMMANDS_HPP
