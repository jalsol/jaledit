#include "keybind/node.hpp"

namespace keybind {

void Node::set_parent(Node* parent) { m_parent = parent; }

Node* Node::get_parent() const { return m_parent; }

std::array<Node*, constants::char_limit>& Node::get_children() {
    return m_children;
}

std::array<Node*, constants::char_limit> Node::get_children() const {
    return m_children;
}

} // namespace keybind