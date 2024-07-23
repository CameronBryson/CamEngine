//
// Created by cam on 07/05/24.
//

#ifndef ISPARSESET_HPP
#define ISPARSESET_HPP
#include <cstddef>
#include <vector>
class ISparseSet {
public:
    virtual ~ISparseSet() = default;
    virtual void removeItem(unsigned short ID) = 0;
    virtual bool hasItem(unsigned short ID) = 0;
    [[nodiscard]] virtual std::size_t getSize() const = 0;
    virtual std::vector<unsigned short> getIDS() = 0;
};
#endif //ISPARSESET_HPP
