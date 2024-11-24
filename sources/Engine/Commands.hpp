#pragma once
#include "Engine/ICommand.hpp"
#include "Engine/Storage/SparseSet.hpp"
#include <deque>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <tuple>

template <typename T, typename... Args>
class AddComponentCommand final : public ICommand
{
public:
    AddComponentCommand(SparseSet<T>& sparse_set, const unsigned short id, Args&&... componentArgs)
      : mSparseSet(sparse_set), mID(id), mComponentArgs(std::forward_as_tuple(componentArgs...))
    {

    }

    void execute() override
    {
	    std::apply([this](auto&&... args) { mSparseSet.addItem(mID, std::forward<decltype(args)>(args)...); }, mComponentArgs);
    }

private:
    SparseSet<T>& mSparseSet;
    unsigned short mID;
    std::tuple<std::decay_t<Args>...> mComponentArgs;
};

template <typename T>
class RemoveComponentCommand final : public ICommand
{
public:
    RemoveComponentCommand(SparseSet<T>& sparse_set, const unsigned short id) : mSparseSet(sparse_set), mID(id)
    {
    }

    void execute() override
    {
		mSparseSet.removeItem(mID);
    }

private:
    SparseSet<T>& mSparseSet;
    unsigned short mID;
};


class DeleteEntityCommand final : public ICommand
{
public:
    DeleteEntityCommand(std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>& sparse_sets,
                          std::vector<unsigned short>& free_i_ds, std::vector<unsigned short>& entities,
                          const unsigned short id)
        : mSparseSets(sparse_sets), mFreeIDs(free_i_ds), mEntities(entities), mID(id)
    {
    }

    void execute() override
    {
	    for( const auto& sparse_set : mSparseSets )
	    {
	        if( sparse_set.second->hasItem(mID) )
	        {
		    sparse_set.second->removeItem(mID);
	        }
	    }
	    mEntities.erase(std::remove(mEntities.begin(), mEntities.end(), mID), mEntities.end());
	    mFreeIDs.emplace_back(mID);
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>& mSparseSets;
    std::vector<unsigned short>& mFreeIDs;
    std::vector<unsigned short>& mEntities;
    unsigned short mID;
};
