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
        mSparse.resize(settings::max_entities);
	    //mDense.resize(settings::max_entities);
	    //mItems.resize(settings::max_entities);
        mDense.reserve(settings::max_entities);
        mItems.reserve(settings::max_entities);
    }

    ~SparseSet() override
    {
        printf("Sparse set destroyed of type: %s\n", typeid(T).name());
    }

    void addItem(const unsigned short id, T&& component_data)
    {
        assert(id < settings::max_entities && "ID is out of range.");
        assert(mSize< settings::max_entities && "Exceeding maximum entities.");
        if (id < settings::max_entities)
        {
	        if( mSize >= mDense.size() )
	        {
		    mDense.emplace_back(id);
		    mItems.emplace_back(std::move(component_data));
	        }
	        else
	        {
		    mDense[mSize] = id;
		    mItems[mSize] = std::move(component_data);
	        }
            //mDense[mSize] = id;
	    //mDense.emplace(mDense.begin() + mSize);
	    //mItems.emplace(mItems.begin() + mSize);
	        //mDense.push_back(id);
            //mItems[mSize] = std::move(component_data);
		    //mItems.push_back(std::move(component_data));
            mSparse[id] = mSize;
            ++mSize;
        }
    }

    void removeItem(const unsigned short id) override
    {
        assert(id < settings::max_entities && "ID is out of range.");
        assert(mSize > 0 && "Sparse set is empty, cannot remove item.");
        if (mSize > 0 && id < settings::max_entities)
        {
            const auto last_id = mDense[mSize - 1];
            auto index_to_remove = mSparse[id];
            mDense[index_to_remove] = last_id;
            mItems[index_to_remove] = std::move(mItems[mSize - 1]);
            mSparse[last_id] = index_to_remove;
            --mSize;
        }
    }

    [[nodiscard]] bool hasItem(const unsigned short id) const override
    {
        return id < settings::max_entities && mSparse[id] < mSize && mDense[mSparse[id]] == id;
    }

    T& get_item(const unsigned short id)
    {
        assert(id < settings::max_entities && "ID is out of range.");
        assert(mSparse[id] < mSize && "ID does not exist in the sparse set.");
        return mItems[mSparse[id]];
    }

    [[nodiscard]] int getSize() const override
    {
        return mSize;
    }

    [[nodiscard]] std::vector<unsigned short> getIDs() const override
    {
        auto ids = std::vector<unsigned short>(mDense.begin(), mDense.begin() + mSize);
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
    std::vector<unsigned short> mDense;
    std::vector<unsigned short> mSparse;
    std::vector<T> mItems;
    int mSize = 0;
};
