#pragma once
#include "Node.hpp"
#include <vector>
class decorator_node : public node{
public:
    enum behavior_status update() override;
};