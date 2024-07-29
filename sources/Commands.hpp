#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <deque>
#include <bitset>

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

class DeleteEntityCommand : public ICommand {
public:
    DeleteEntityCommand(std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>& sparseSets, std::vector<unsigned short>& freeIDs, std::vector<unsigned short>& entities, unsigned short ID)
        : m_SparseSets(sparseSets), m_freeIDs(freeIDs), m_Entities(entities), m_ID(ID) {}

    void Execute() override {
        for (auto& sparseSet : m_SparseSets) {
            if(sparseSet.second->hasItem(m_ID)) {
                sparseSet.second->removeItem(m_ID);
            }
        }
        auto it = std::find(m_Entities.begin(), m_Entities.end(), m_ID);
        if (it != m_Entities.end()) {
            m_Entities.erase(it);
        }
        m_freeIDs.push_back(m_ID);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>& m_SparseSets;
    std::vector<unsigned short>& m_freeIDs;
    std::vector<unsigned short>& m_Entities;
    unsigned short m_ID;
};
