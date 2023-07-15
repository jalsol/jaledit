#include "keybind/node.hpp"

namespace keybind {

Node::~Node() {
    for (auto* child : m_children) {
        delete child;
    }
}

void Node::set_parent(Node* parent) { m_parent = parent; }

Node*& Node::parent() { return m_parent; }

Node*& Node::child(char c) { return m_children[static_cast<size_t>(c)]; }

std::array<Node*, constants::char_limit>& Node::children() {
    return m_children;
}

const std::array<Node*, constants::char_limit>& Node::children() const {
    return m_children;
}

} // namespace keybind