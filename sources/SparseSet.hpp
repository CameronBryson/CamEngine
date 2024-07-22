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
    SparseSet() {
        printf("Sparse set created of type: %s\n", typeid(T).name());
    }
    ~SparseSet() override {
        printf("Sparse set destroyed of type: %s\n", typeid(T).name());
    }

    void addItem(unsigned short ID, const T& componentData);
    void removeItem(unsigned short ID) override;
    void resetComponent(unsigned short ID) override;
    bool hasItem(unsigned short ID) override;
    T& getItem(unsigned short ID);
    size_t getSize() const override;
    std::vector<unsigned short> getIDS() override;

private:
    std::vector<unsigned short> m_Dense;
    std::array<unsigned short, Settings::MAX_COMPONENTS> m_Sparse;
    std::array<T, Settings::MAX_COMPONENTS> m_Items;
    std::vector<unsigned short> m_AvailableIndices; // Tracks dead component indices for reuse

    bool isIndexValid(unsigned short ID) const;
};

template<class T>
void SparseSet<T>::addItem(unsigned short ID, const T& componentData) {
    if (!m_AvailableIndices.empty()) {
        // Reuse a dead component slot
        unsigned short reuseIndex = m_AvailableIndices.back();
        m_AvailableIndices.pop_back();
        m_Dense[reuseIndex] = ID;
        m_Items[ID] = std::move(componentData);
        m_Sparse[ID] = reuseIndex;
    } else {
        // Add a new component
        const auto pos = m_Dense.size();
        m_Dense.push_back(ID);
        m_Items[ID] = std::move(componentData);
        m_Sparse[ID] = pos;
    }
}

template<class T>
void SparseSet<T>::removeItem(unsigned short ID) {
    // Instead of removing, reset and mark as dead
    resetComponent(ID);
}

template<class T>
void SparseSet<T>::resetComponent(unsigned short ID) {
    if (hasItem(ID)) {
        m_Items[ID] = T{}; // Reset to dead state
        m_AvailableIndices.push_back(m_Sparse[ID]); // Mark index as available for reuse
    }
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

template<class T>
bool SparseSet<T>::isIndexValid(unsigned short ID) const {
    return std::find(m_AvailableIndices.begin(), m_AvailableIndices.end(), ID) == m_AvailableIndices.end();
}
#endif //SPARSESET_HPP
