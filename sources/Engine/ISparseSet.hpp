#pragma once
#include <cstddef>
#include <vector>

class i_sparse_set
{
public:
    virtual ~i_sparse_set() = default;
    virtual void remove_item(unsigned short id) = 0;
    [[nodiscard]] virtual bool has_item(unsigned short id) const = 0;
    [[nodiscard]] virtual std::size_t get_size() const = 0;
    [[nodiscard]] virtual std::vector<unsigned short> get_ids() const = 0;
    [[nodiscard]] virtual std::vector<unsigned short> get_intersection(
        const std::vector<unsigned short>& other) const = 0;
};
