#pragma once
#include <cstddef>
#include <vector>

class ISparseSet
{
public:
    virtual ~ISparseSet() = default;
    virtual void removeItem(unsigned short id) = 0;
    [[nodiscard]] virtual bool hasItem(unsigned short id) const = 0;
    [[nodiscard]] virtual std::size_t getSize() const = 0;
    [[nodiscard]] virtual std::vector<unsigned short> getIDs() const = 0;
    [[nodiscard]] virtual std::vector<unsigned short> getIntersection(
        const std::vector<unsigned short>& other) const = 0;
};
