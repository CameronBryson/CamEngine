//
// Created by cam on 07/05/24.
//

#ifndef SPARSESET_HPP
#define SPARSESET_HPP
// Modification in SparseSet.hpp to incorporate dead components

#include <algorithm>
#include <array>
#include <cstdio>
#include <typeindex>
#include <typeinfo>
#include <vector>
#include "GameSettings.hpp"
#include "ISparseSet.hpp"

template<class T>
class SparseSet final : public ISparseSet {
public:
    SparseSet() : m_Sparse() { printf("Sparse set created of type: %s\n", typeid(T).name()); }
    ~SparseSet() override {
        printf("Sparse set destroyed of type: %s\n", typeid(T).name());
        m_Items.empty();
    }

    void addItem(unsigned short ID, T componentData);
    void removeItem(unsigned short ID) override;
    bool hasItem(unsigned short ID) override;
    T& getItem(unsigned short ID);
    [[nodiscard]] std::size_t getSize() const override;
    std::vector<unsigned short> getIDS() override;

private:
    std::vector<unsigned short> m_Dense;
    std::array<unsigned short, Settings::MAX_ENTITIES> m_Sparse;
    std::array<T, Settings::MAX_ENTITIES> m_Items;
};

template<class T>
void SparseSet<T>::addItem(unsigned short ID, T componentData) {
        // Add a new component
    const auto pos = m_Dense.size();
    m_Dense.push_back(ID);
    m_Items[ID] = std::move(componentData);
    //m_Items[ID] = componentData;
    m_Sparse[ID] = pos;
}

template<class T>
void SparseSet<T>::removeItem(unsigned short ID) {
    // Instead of removing, reset and mark as dead
    const auto last = m_Dense.back();
    std::swap(m_Dense.back(), m_Dense[m_Sparse[ID]]);
    std::swap(m_Items.back(), m_Items[m_Sparse[ID]]);
    std::swap(m_Sparse[last], m_Sparse[ID]);
    m_Dense.pop_back();
}


template<class T>
bool SparseSet<T>::hasItem(unsigned short ID) {
    return ID < m_Sparse.size() && m_Sparse[ID] < m_Dense.size() && m_Dense[m_Sparse[ID]] == ID;
}

template<class T>
T& SparseSet<T>::getItem(unsigned short ID) {
    return m_Items[m_Sparse[ID]];
}

template<class T>
size_t SparseSet<T>::getSize() const {
    return m_Dense.size();
}

template<class T>
std::vector<unsigned short> SparseSet<T>::getIDS() {
    return m_Dense;
}

#endif //SPARSESET_HPP
