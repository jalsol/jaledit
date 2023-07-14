#include "keybind/node.hpp"

namespace keybind {

void Node::set_parent(Node* parent) { m_parent = parent; }

Node* Node::parent() const { return m_parent; }

std::array<Node*, constants::char_limit>& Node::children() {
    return m_children;
}

const std::array<Node*, constants::char_limit>& Node::children() const {
    return m_children;
}

} // namespace keybind