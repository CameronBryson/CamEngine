#pragma once
#include "GameSettings.hpp"
#include "ISparseSet.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iterator>
#include <typeinfo>
#include <vector>
#include <stdexcept>

template <class T>
class SparseSet final : public ISparseSet
{
  public:
    SparseSet();

    void addItem(unsigned short entityID, T component);
    void removeItem(unsigned short entityID) override;
    bool hasItem(unsigned short entityID) override;
    T& get_item(unsigned short entityID);
    int getSize() const override;
    std::vector<unsigned short> getIDs() const override;
    std::vector<unsigned short> getIntersection(std::vector<unsigned short> obj) override;
  private:
    std::vector<unsigned short> m_Dense;
    std::vector<unsigned short> m_Sparse;
    std::vector<T> m_Items;
};

template <class T>
SparseSet<T>::SparseSet()
{
    m_Sparse.resize(settings::max_entities);
    //m_Dense.resize(MAX_ENTITIES);
    //m_Items.resize(MAX_ENTITIES);
    m_Dense.reserve(settings::max_entities);
    m_Items.reserve(settings::max_entities);
}

template <class T>
void SparseSet<T>::addItem(unsigned short entityID, T component)
{
    if( entityID >= settings::max_entities )
    {
	throw std::out_of_range("Exceeded maximum capacity of SparseSet.");
    }
    m_Dense.emplace_back(entityID);
    m_Items.emplace_back(component);
    //m_Items.emplace_back(std::forward<Args>(componentArgs)...);
    m_Sparse[entityID] = m_Dense.size() - 1;
}


template <class T>
void SparseSet<T>::removeItem(const unsigned short entityID)
{
    unsigned index = m_Sparse[entityID];
    unsigned lastIndex = m_Dense.size() - 1;

    if( index != lastIndex )
    {
	std::swap(m_Dense[index], m_Dense[lastIndex]);
	std::swap(m_Items[index], m_Items[lastIndex]);
	m_Sparse[m_Dense[index]] = index;
    }

    // Remove the last item
    m_Dense.pop_back();
    m_Items.pop_back();

    m_Sparse[entityID] = settings::max_entities; // Mark as removed or invalid
}

template <class T>
bool SparseSet<T>::hasItem(const unsigned short entityID)
{
    return m_Sparse[entityID] < m_Dense.size() && m_Dense[m_Sparse[entityID]] == entityID;
}

template <class T>
T& SparseSet<T>::get_item(const unsigned short entityID)
{
    auto index = m_Sparse[entityID];
    if( index >= getSize() )
    {
	throw std::out_of_range("Entity not found in SparseSet.");
    }
    return m_Items[index];
}

template <class T>
int SparseSet<T>::getSize() const
{
    return m_Dense.size();
}

template <class T>
std::vector<unsigned short> SparseSet<T>::getIDs() const
{
    return m_Dense;
}

template <class T>
std::vector<unsigned short> SparseSet<T>::getIntersection(std::vector<unsigned short> obj)
{
    auto ids = getIDs();
    std::sort(ids.begin(), ids.end());
    std::sort(obj.begin(), obj.end());
    std::vector<unsigned short> intersection;
    intersection.reserve(std::min(ids.size(), obj.size()));
    std::set_intersection(ids.begin(), ids.end(), obj.begin(), obj.end(), std::back_inserter(intersection));
    return intersection;
}
