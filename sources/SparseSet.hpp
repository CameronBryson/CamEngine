#pragma once
#include <algorithm>
#include <vector>
#include <cstdio>
#include <typeinfo>
#include <cassert>
#include "GameSettings.hpp"
#include "ISparseSet.hpp"
#include <unordered_set>

template<class T>
class SparseSet final : public ISparseSet {
public:
    SparseSet() {
        printf("Sparse set created of type: %s\n", typeid(T).name());
        m_Dense.resize(Settings::MAX_ENTITIES);
        m_Sparse.resize(Settings::MAX_ENTITIES);
        m_Items.resize(Settings::MAX_ENTITIES);
    }
    ~SparseSet() override {
        printf("Sparse set destroyed of type: %s\n", typeid(T).name());
    }

    void addItem(unsigned short ID, T componentData) {
        assert(ID < Settings::MAX_ENTITIES && "ID is out of range.");
        assert(m_Size < Settings::MAX_ENTITIES && "Exceeding maximum entities.");
        if (ID < Settings::MAX_ENTITIES) {
            m_Dense[m_Size] = ID;
            m_Items[m_Size] = std::move(componentData);
            m_Sparse[ID] = m_Size;
            ++m_Size;
        }
    }

    void removeItem(unsigned short ID) override {
        assert(ID < Settings::MAX_ENTITIES && "ID is out of range.");
        assert(m_Size > 0 && "Sparse set is empty, cannot remove item.");
        if (m_Size > 0 && ID < Settings::MAX_ENTITIES) {
            auto lastID = m_Dense[m_Size - 1];
            auto indexToRemove = m_Sparse[ID];
            m_Dense[indexToRemove] = lastID;
            m_Items[indexToRemove] = std::move(m_Items[m_Size - 1]);
            m_Sparse[lastID] = indexToRemove;
            --m_Size;
        }
    }

    [[nodiscard]] bool hasItem(unsigned short ID) const override {
        return ID < Settings::MAX_ENTITIES && m_Sparse[ID] < m_Size && m_Dense[m_Sparse[ID]] == ID;
    }

    T& getItem(unsigned short ID) {
        assert(ID < Settings::MAX_ENTITIES && "ID is out of range.");
        assert(m_Sparse[ID] < m_Size && "ID does not exist in the sparse set.");
        return m_Items[m_Sparse[ID]];
    }

    [[nodiscard]] std::size_t getSize() const override {
        return m_Size;
    }

    [[nodiscard]] std::vector<unsigned short> getIDS() const override {
        return {m_Dense.begin(), m_Dense.begin() + m_Size};
    }

    std::vector<unsigned short> getIntersection(const std::vector<unsigned short>& other) const override {
        auto IDS = getIDS();
        std::vector<unsigned short> intersection;
        intersection.reserve(std::min(IDS.size(), other.size()));
        std::set_intersection(IDS.begin(), IDS.end(), other.begin(), other.end(), std::back_inserter(intersection));
        return intersection;
    }

private:
    std::vector<unsigned short> m_Dense;
    std::vector<unsigned short> m_Sparse;
    std::vector<T> m_Items;
    std::size_t m_Size = 0;
};