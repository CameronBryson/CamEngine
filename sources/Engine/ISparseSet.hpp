#pragma once
#include <vector>

class ISparseSet
{
public:
    virtual ~ISparseSet() = default;
    virtual void removeItem(unsigned short id) = 0;
    [[nodiscard]] virtual bool hasItem(unsigned short id) = 0;
    [[nodiscard]] virtual int getSize() const = 0;
    [[nodiscard]] virtual std::vector<unsigned short> getIDs() const  = 0;
    [[nodiscard]] virtual std::vector<unsigned short> getIntersection(
        std::vector<unsigned short> other) = 0;
};
