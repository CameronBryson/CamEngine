#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ISparseSet.hpp"
#include "SparseSet.hpp"


#include <cstdint>
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ThreadPool.hpp"
class Registry {
public:
    Registry() {
        printf("Registry created\n");
        pool = new ThreadPool;
        for (int i = 0; i < Settings::MAX_ENTITIES; ++i) {
            m_freeIDs.push_back(i);
        }
    };
    ~Registry() {
        delete pool;
        for (const auto sparseSet: m_SparseSets) {
            delete sparseSet.second;
        }
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
        for (const auto sparseSet : m_SparseSets)
        {
            if (sparseSet.second->hasItem(ID))
                sparseSet.second->removeItem(ID);
        }
        m_freeIDs.push_back(ID); //Add freed ID to list of available
    };


public:
    template<typename T>
    void createSparseSet();

    template<typename T>
    SparseSet<T>* getSparseSet();

    template<typename T>
    void addComponent(unsigned short ID, T item);

    template<typename T>
    T& getComponent(unsigned short ID);

    template<typename T>
    std::vector<unsigned short> getEntityIDS();

    template<typename T>
    std::vector<T> &getComponents();

    template<typename T>
    bool hasComponent(unsigned short ID);

    template<typename T>
    void removeComponent(unsigned short ID);

    ThreadPool* pool;





private:
    std::deque<unsigned short> m_freeIDs;
    //std::vector<unsigned short> m_Entities;
    std::unordered_map<const char *, ISparseSet*> m_SparseSets;
};
template<class T>
void Registry::createSparseSet() {
    m_SparseSets[typeid(T).name()] = new SparseSet<T>();
}
template<typename T>
SparseSet<T>* Registry::getSparseSet() {
    return static_cast<SparseSet<T>*>(m_SparseSets[typeid(T).name()]);
}
template<typename T>
void Registry::addComponent(unsigned short ID, T item) {
    getSparseSet<T>()->addItem(ID, item);
}
template<typename T>
T &Registry::getComponent(unsigned short ID) {
    const auto set = getSparseSet<T>();
    return set->getItem(ID);
}
template<typename T>
std::vector<unsigned short> Registry::getEntityIDS() {
    const auto set = getSparseSet<T>();
    return set->m_Dense;
}
template<typename T>
std::vector<T> &Registry::getComponents() {
    const auto set = getSparseSet<T>();
    return set->m_Items;
}
template<typename T>
bool Registry::hasComponent(unsigned short ID) {
    const auto set = getSparseSet<T>();
    return set->hasItem(ID);
}
template<typename T>
void Registry::removeComponent(unsigned short ID) {
    const auto set = getSparseSet<T>();
    set->removeItem(ID);
}


#endif // REGISTRY_HPP
