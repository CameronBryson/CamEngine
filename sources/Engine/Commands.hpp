#pragma once
#include "SparseSet.hpp"

#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>

class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
};

template <typename T>
class AddComponentCommand final : public ICommand
{
public:
    AddComponentCommand(SparseSet<T>& sparse_set, const unsigned short id, T component_data)
        : m_sparse_set_(sparse_set), m_id_(id), m_component_data_(std::move(component_data))
    {
    }

    void execute() override
    {
        m_sparse_set_.addItem(m_id_, m_component_data_);
    }

private:
    SparseSet<T>& m_sparse_set_;
    unsigned short m_id_;
    T m_component_data_;
};

template <typename T>
class RemoveComponentCommand final : public ICommand
{
public:
    RemoveComponentCommand(SparseSet<T>& sparse_set, const unsigned short id) : m_sparse_set_(sparse_set), m_id_(id)
    {
    }

    void execute() override
    {
        m_sparse_set_.remove_item(m_id_);
    }

private:
    SparseSet<T>& m_sparse_set_;
    unsigned short m_id_;
};


class DeleteEntityCommand final : public ICommand
{
public:
    DeleteEntityCommand(std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>& sparse_sets,
                          std::vector<unsigned short>& free_i_ds, std::vector<unsigned short>& entities,
                          const unsigned short id)
        : m_sparse_sets_(sparse_sets), m_free_i_ds_(free_i_ds), m_entities_(entities), m_id_(id)
    {
    }

    void execute() override
    {
        for (const auto& sparse_set : m_sparse_sets_)
        {
            if (sparse_set.second->hasItem(m_id_))
            {
                sparse_set.second->removeItem(m_id_);
            }
        }
        const auto it = std::find(m_entities_.begin(), m_entities_.end(), m_id_);
        if (it != m_entities_.end())
        {
            m_entities_.erase(it);
        }
        m_free_i_ds_.push_back(m_id_);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>& m_sparse_sets_;
    std::vector<unsigned short>& m_free_i_ds_;
    std::vector<unsigned short>& m_entities_;
    unsigned short m_id_;
};
