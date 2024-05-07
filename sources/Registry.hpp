#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ISparseSet.hpp"
#include "SparseSet.hpp"


#include <cstdint>
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>
class Registry {
public:
    Registry() {

    };
    ~Registry() {

    }
    
    unsigned short createEntity();
    void deleteEntity(unsigned short ID);
    template<class T>
    void createSparseSet();


private:
    std::deque<unsigned short> m_freeIDs;
    std::unordered_map<const char*, std::shared_ptr<ISparseSet>> m_SparseSets;
};
template<class T>
void Registry::createSparseSet() {
    m_SparseSets[typeid(T).name()] = std::make_shared<SparseSet<T>>();
}

#endif // REGISTRY_HPP
