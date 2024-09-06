#include "Node.hpp"
node::node(node *parent) : parent(parent), status(behavior_status::running) {

}
void node::add_child(node *child) {
    children.push_back(child);
}
behavior_status node::get_status() const {
    return status;
}
int node::get_child_count() const {
    return children.size();
}
node* node::get_child(int index) const {
    return children[index];
}
node* node::get_parent() const {
    return parent;
}

