#pragma once
#include "SparseSet.hpp"
#include "EventHandler.hpp"
#include "Commands.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <functional>
#include <memory>
#include <numeric>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>




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
    T& getComponent(unsigned short id) const;

    template <typename T>
    [[nodiscard]] bool hasComponent(unsigned short id) const;

    template <typename T>
    void removeComponent(unsigned short id);

    template <typename... T>
    [[nodiscard]] std::vector<unsigned short> getEntityIDs() const;

    template <typename T>
    SparseSet<T>& getSparseSet() const;

    template <typename T>
    [[nodiscard]] bool hasSparseSet() const;

private:
    std::vector<unsigned short> mFreeIDs;
    std::vector<unsigned short> mEntities;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> mSparseSets;
    //some kind of key storage
};

#include "Registry.tpp"
