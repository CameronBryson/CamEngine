#pragma once
#include "ISparseSet.hpp"
#include "GameSettings.hpp"
#include "EventHandler.hpp"
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

    template <typename... Args>
    void addItem(unsigned short entityID, Args&&... componentArgs);
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
#include "SparseSet.tpp"

