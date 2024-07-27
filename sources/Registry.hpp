#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ISparseSet.hpp"
#include "SparseSet.hpp"
#include "Commands.hpp"

#include <cstdint>
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <queue>
#include <algorithm>
#include <cassert>
#include <bitset>

class Registry {
public:
    Registry() {
        printf("Registry created\n");
        for (int i = 1; i < Settings::MAX_ENTITIES; ++i) {
            m_freeIDs.push_back(i);
        }
    };
    ~Registry() {
        printf("Registry destroyed\n");
    }

public:
    unsigned short createEntity() {
        assert(!m_freeIDs.empty() && "No more entities available.");
        const unsigned short ID = m_freeIDs.front();
        m_freeIDs.pop_front();
        m_Entities.set(ID);
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
    std::deque<unsigned short> m_freeIDs;
    std::bitset<Settings::MAX_ENTITIES> m_Entities;
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
    assert(m_Entities[ID] && "Entity does not exist.");
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
            if (m_Entities[i]) {
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
        result = sparseSets[i]->getIntersection(*sparseSets[i]);
    }

    return result;
}

template<typename T>
bool Registry::hasComponent(unsigned short ID) const {
    return getSparseSet<T>().hasItem(ID);
}

template<typename T>
void Registry::removeComponent(unsigned short ID) {
    assert(m_Entities[ID] && "Entity does not exist.");
    assert(hasComponent<T>(ID) && "Entity does not have component.");
    auto& set = getSparseSet<T>();
    commandQueue.push(std::make_unique<RemoveComponentCommand>(set, ID));
}

#endif // REGISTRY_HPP