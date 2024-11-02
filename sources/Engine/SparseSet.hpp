#pragma once
#include "GameSettings.hpp"
#include "ISparseSet.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iterator>
#include <typeinfo>
#include <vector>

template <class T>
class SparseSet final : public ISparseSet
{
public:
    SparseSet()
    {
        printf("Sparse set created of type: %s\n", typeid(T).name());
        m_dense_.resize(settings::max_entities);
        m_sparse_.resize(settings::max_entities);
        m_items_.resize(settings::max_entities);
    }

    ~SparseSet() override
    {
        printf("Sparse set destroyed of type: %s\n", typeid(T).name());
    }

    void addItem(const unsigned short id, T component_data)
    {
        assert(id < settings::max_entities && "ID is out of range.");
        assert(m_size_< settings::max_entities && "Exceeding maximum entities.");
        if (id < settings::max_entities)
        {
            m_dense_[m_size_] = id;
            m_items_[m_size_] = std::move(component_data);
            m_sparse_[id] = m_size_;
            ++m_size_;
        }
    }

    void removeItem(const unsigned short id) override
    {
        assert(id < settings::max_entities && "ID is out of range.");
        assert(m_size_ > 0 && "Sparse set is empty, cannot remove item.");
        if (m_size_ > 0 && id < settings::max_entities)
        {
            const auto last_id = m_dense_[m_size_ - 1];
            auto index_to_remove = m_sparse_[id];
            m_dense_[index_to_remove] = last_id;
            m_items_[index_to_remove] = std::move(m_items_[m_size_ - 1]);
            m_sparse_[last_id] = index_to_remove;
            --m_size_;
        }
    }

    [[nodiscard]] bool hasItem(const unsigned short id) const override
    {
        return id < settings::max_entities && m_sparse_[id] < m_size_ && m_dense_[m_sparse_[id]] == id;
    }

    T& get_item(const unsigned short id)
    {
        assert(id < settings::max_entities && "ID is out of range.");
        assert(m_sparse_[id] < m_size_ && "ID does not exist in the sparse set.");
        return m_items_[m_sparse_[id]];
    }

    [[nodiscard]] std::size_t getSize() const override
    {
        return m_size_;
    }

    [[nodiscard]] std::vector<unsigned short> getIDs() const override
    {
        auto ids = std::vector<unsigned short>(m_dense_.begin(), m_dense_.begin() + m_size_);
        std::sort(ids.begin(), ids.end());
        return ids;
    }

    [[nodiscard]] std::vector<unsigned short> getIntersection(const std::vector<unsigned short>& other) const override
    {
        auto ids = getIDs();
        std::vector<unsigned short> intersection;
        intersection.reserve(std::min(ids.size(), other.size()));
        std::set_intersection(ids.begin(), ids.end(), other.begin(), other.end(), std::back_inserter(intersection));
        return intersection;
    }

private:
    std::vector<unsigned short> m_dense_;
    std::vector<unsigned short> m_sparse_;
    std::vector<T> m_items_;
    unsigned short m_size_ = 0;
};
