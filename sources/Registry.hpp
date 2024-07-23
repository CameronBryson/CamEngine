#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ISparseSet.hpp"
#include "SparseSet.hpp"


#include <cstdint>
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <queue>

#include <algorithm>

#include "Commands.hpp"
class Registry {
public:
    Registry() : m_Locks(Settings::MAX_ENTITIES) {
        printf("Registry created\n");
        for (int i = 0; i < Settings::MAX_ENTITIES; ++i) {
            m_freeIDs.push_back(i);
        }
    };
    ~Registry() {
        // for (const auto sparseSet: m_SparseSets) {
        //     delete sparseSet.second;
        // }
        printf("Registry destroyed\n");
    }

public:
    unsigned short createEntity() {
        const unsigned short ID = m_freeIDs.front();
        m_freeIDs.pop_front();
        //m_Entities.push_back(ID);
        return ID;

    };
    void deleteEntity(unsigned short ID) {
        for (const auto& sparseSet : m_SparseSets)
        {
            if (sparseSet.second->hasItem(ID))
                sparseSet.second->removeItem(ID);
        }
        m_freeIDs.push_back(ID); //Add freed ID to list of available
    };
    std::unique_lock<std::mutex> lockEntity(unsigned short ID) {
        return std::unique_lock<std::mutex>(m_Locks[ID]);
    }
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
    T& getComponent(unsigned short ID);

    template<typename T>
    bool hasComponent(unsigned short ID);

    template<typename T>
    void removeComponent(unsigned short ID);

    template<typename... T>
    std::vector<unsigned short> getEntityIDS();

private:
    std::deque<unsigned short> m_freeIDs;
    //figure out way to make sure entities and locks match up
    //std::vector<unsigned short> m_Entities;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> m_SparseSets;
    std::queue<std::unique_ptr<ICommand>> commandQueue;
    std::vector<std::mutex> m_Locks;
    template<typename T>
    SparseSet<T>& getSparseSet();

};
template<class T>
void Registry::createSparseSet() {
    m_SparseSets[std::type_index(typeid(T))] = std::make_unique<SparseSet<T>>();
}
template<typename T>
SparseSet<T>&  Registry::getSparseSet() {
    return *static_cast<SparseSet<T>*>(m_SparseSets[std::type_index(typeid(T))].get());
}
template<typename T>
void Registry::addComponent(unsigned short ID, const T &componentData) {
    //Add a addcomponent command to the queue to be processed later
    //getSparseSet<T>().addItem(ID, componentData);
    commandQueue.push(std::make_unique<AddComponentCommand<T>>(getSparseSet<T>(), ID, componentData));
}
template<typename T>
T &Registry::getComponent(unsigned short ID) {
    return getSparseSet<T>().getItem(ID);
}
template<typename... T>
std::vector<unsigned short> Registry::getEntityIDS() {
    // Optimization for single component type
    if (sizeof...(T) == 1) return (getSparseSet<T>().getIDS(), ...);
    std::vector<unsigned short> result, shortestIDs;
    std::size_t minSize = std::numeric_limits<std::size_t>::max();

    // Combine finding the shortest vector and checking components into a single loop
    ([&](const auto& vec) {
        if (vec.size() < minSize) {
            minSize = vec.size();
            shortestIDs = vec;
        }
        for (const auto ID: shortestIDs) {
            if ((hasComponent<T>(ID) && ...)) { // Fold expression to check all components
                result.push_back(ID);
            }
        }
    }(getSparseSet<T>().getIDS()), ...);

    return result;
}
template<typename T>
bool Registry::hasComponent(unsigned short ID) {
    return getSparseSet<T>().hasItem(ID);
}
template<typename T>
void Registry::removeComponent(unsigned short ID) {//Add a removecomponent command to the queue to be processed later
    //getSparseSet<T>().removeItem(ID);
    commandQueue.push(std::make_unique<RemoveComponentCommand<T>>(getSparseSet<T>(), ID));
}

#endif // REGISTRY_HPP
