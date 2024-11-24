#pragma once
#include "SparseSet.hpp"


#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "Commands.hpp"




class Registry
{
public:

    Registry();
    ~Registry();

    unsigned short createEntity();
    void deleteEntity(unsigned short id);

public:
    template <typename T>
    void createSparseSet();

    template <typename T,typename... Args>
    void addComponent(unsigned short id, Args&&... componentArgs);


    template <typename T>
    T& getComponent(unsigned short id);

    template <typename T>
    [[nodiscard]] bool hasComponent(unsigned short id);

    template <typename T>
    void removeComponent(unsigned short id);

    template <typename... T>
    [[nodiscard]] std::vector<unsigned short> getEntityIDs();
    template <typename T>
    SparseSet<T>& getSparseSet();

    template <typename T>
    [[nodiscard]] bool hasSparseSet() const;

private:
    std::vector<unsigned short> mFreeIDs;
    std::vector<unsigned short> mEntities;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> mSparseSets;
    //some kind of key storage
};

#include "Registry.tpp"
