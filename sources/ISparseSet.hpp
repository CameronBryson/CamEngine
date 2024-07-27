#pragma once
#include <cstddef>
#include <vector>
class ISparseSet {
public:
    virtual ~ISparseSet() = default;
    virtual void removeItem(unsigned short ID) = 0;
    [[nodiscard]] virtual bool hasItem(unsigned short ID) const = 0;
    [[nodiscard]] virtual std::size_t getSize() const = 0;
    [[nodiscard]] virtual std::vector<unsigned short> getIDS() const = 0;
    virtual std::vector<unsigned short> getIntersection(const ISparseSet& other) const = 0;
};
