#pragma once
#include "Node.hpp"
#include <vector>
class action_node : public node{
public:
    enum behavior_status update() override;
};
