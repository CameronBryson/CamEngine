#pragma once
#include "INode.hpp"
#include <vector>
class selector_node: public i_node{
public:
    void start() override;
    status update(float dt) override;
    void terminate(status status) override;
private:
    std::vector<i_node> children;
};
