#pragma once
#include "Node.hpp"
#include <vector>
class selector_node: public node{
public:
    enum behavior_status update() override;
};
