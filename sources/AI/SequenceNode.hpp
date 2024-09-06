#pragma once
#include "Node.hpp"
#include <vector>
class sequence_node : public node{
public:
    enum behavior_status update() override;
};
