#pragma once
#include <vector>
enum class behavior_status{
    success,
    failure,
    running
};
class node{
public:
    node(node* parent = nullptr);
    void add_child(node* child);
    behavior_status get_status() const;
    int get_child_count() const;
    node* get_child(int index) const;
    node* get_parent() const;

    virtual enum behavior_status update() = 0;

private:
    behavior_status status;
    node* parent;
    std::vector<node*> children;
};