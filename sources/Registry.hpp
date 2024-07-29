#pragma once
#include "ISparseSet.hpp"
#include "SparseSet.hpp"
#include "Commands.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Registry {
public:
    Registry() {
        printf("Registry created\n");
        m_Entities.reserve(Settings::MAX_ENTITIES);
        m_freeIDs.resize(Settings::MAX_ENTITIES-1);
        std::iota(m_freeIDs.begin(), m_freeIDs.end(), 1);

    };
    ~Registry() {
        ProcessCommands();
        printf("Registry destroyed\n");
    }

public:
    unsigned short createEntity() {
        assert(!m_freeIDs.empty() && "No more entities available.");
        const unsigned short ID = m_freeIDs.back();
        m_freeIDs.pop_back();
        m_Entities.push_back(ID);
        return ID;
    };
    void deleteEntity(unsigned short ID) {
        commandQueue.push(std::make_unique<DeleteEntityCommand>(m_SparseSets, m_freeIDs, m_Entities, ID));
    };
    void ProcessCommands() {
        while (!commandQueue.empty()) {
            const auto & command = commandQueue.front();
            command->Execute();
            commandQueue.pop();
        }
    }

public:
    template<typename T>
    void createSparseSet();

    template<typename T>
    void addComponent(unsigned short ID, const T& componentData);

    template<typename T>
    T& getComponent(unsigned short ID) const;

    template<typename T>
    [[nodiscard]] bool hasComponent(unsigned short ID) const;

    template<typename T>
    void removeComponent(unsigned short ID);

    template<typename... T>
    [[nodiscard]] std::vector<unsigned short> getEntityIDS() const;

    template<typename T>
    SparseSet<T>& getSparseSet() const;

private:
    std::vector<unsigned short> m_freeIDs;
    std::vector<unsigned short> m_Entities;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_SparseSets;
    std::queue<std::unique_ptr<ICommand>> commandQueue;
};

template<class T>
void Registry::createSparseSet() {
    assert(m_SparseSets.find(std::type_index(typeid(T))) == m_SparseSets.end() && "Error: Sparse set already exists for this type.");
    m_SparseSets[std::type_index(typeid(T))] = std::make_unique<SparseSet<T>>();
}

template<typename T>
SparseSet<T>&  Registry::getSparseSet() const {
    assert(m_SparseSets.find(std::type_index(typeid(T))) != m_SparseSets.end() && "Error: Sparse set does not exist for this type.");
    return *static_cast<SparseSet<T>*>(m_SparseSets.at(std::type_index(typeid(T))).get());
}

template<typename T>
void Registry::addComponent(unsigned short ID, const T &componentData) {
    assert(m_Entities.end() != std::find(m_Entities.begin(), m_Entities.end(), ID) && "Entity does not exist.");
    auto& set = getSparseSet<T>();
    commandQueue.push(std::make_unique<AddComponentCommand<T>>(set, ID, componentData));
}

template<typename T>
T &Registry::getComponent(unsigned short ID) const {
    assert(hasComponent<T>(ID) && "Entity does not have component.");
    return getSparseSet<T>().getItem(ID);
}

template<typename... T>
std::vector<unsigned short> Registry::getEntityIDS() const {
    std::vector<unsigned short> result;
    if(sizeof...(T) == 0) {
        for (unsigned short i = 0; i < Settings::MAX_ENTITIES; ++i) {
            if (std::find(m_Entities.begin(), m_Entities.end(), i) != m_Entities.end()) {
                result.push_back(i);
            }
        }
        return result;
    }
    std::vector<ISparseSet*> sparseSets = {&getSparseSet<T>()...};

    // Initialize result with the first component's IDs if available
    result = sparseSets[0]->getIDS();

    // Find intersection across all component types
    for (size_t i = 1; i < sparseSets.size(); ++i) {
        result = sparseSets[i]->getIntersection(result);
    }

    return result;
}

template<typename T>
bool Registry::hasComponent(unsigned short ID) const {
    return getSparseSet<T>().hasItem(ID);
}

template<typename T>
void Registry::removeComponent(unsigned short ID) {
    assert(m_Entities.end() != std::find(m_Entities.begin(), m_Entities.end(), ID) && "Entity does not exist.");
    assert(hasComponent<T>(ID) && "Entity does not have component.");
    auto& set = getSparseSet<T>();
    commandQueue.push(std::make_unique<RemoveComponentCommand>(set, ID));
}