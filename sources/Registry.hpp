#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ISparseSet.hpp"
#include "SparseSet.hpp"


#include <cstdint>
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>
static constexpr int MAX_IDS = 255;
class Registry {
public:
    Registry() {
        for (int i = 0; i < MAX_IDS; ++i) {
            m_freeIDs.push_back(i);
        }
    };
    ~Registry() {}

public:
    unsigned short createEntity() {
        const unsigned short ID = m_freeIDs.front();
        m_freeIDs.pop_front();
        m_Entities.push_back(ID);
        return ID;

    };
    void deleteEntity(unsigned short ID);

public:
    template<typename T>
    void createSparseSet();

    template<typename T>
    std::shared_ptr<SparseSet<T>> getSparseSet();

    template<typename T>
    void addComponent(unsigned short ID, T item);


private:
    std::vector<unsigned short> m_Entities;
    std::deque<unsigned short> m_freeIDs;
    std::unordered_map<const char *, std::shared_ptr<ISparseSet>> m_SparseSets;
};
template<class T>
void Registry::createSparseSet() {
    m_SparseSets[typeid(T).name()] = std::make_shared<SparseSet<T>>();
}
template<typename T>
std::shared_ptr<SparseSet<T>> Registry::getSparseSet() {
    return std::static_pointer_cast<SparseSet<T>>(m_SparseSets[typeid(T).name()]);
}
template<typename T>
void Registry::addComponent(unsigned short ID, T item) {
    getSparseSet<T>()->addItem(ID,item);
}

#endif // REGISTRY_HPP
