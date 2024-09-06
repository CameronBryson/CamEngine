#pragma once
#include "Node.hpp"
#include <vector>
class condition_node : public node{
public:
    enum behavior_status update() override;
};
