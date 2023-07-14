#pragma once

#include "constants.hpp"
#include "utils.hpp"

#include <array>
#include <concepts>
#include <tuple>

namespace keybind {

class Node {
public:
    void set_parent(Node* parent);
    Node* parent() const;

    std::array<Node*, constants::char_limit>& children();
    const std::array<Node*, constants::char_limit>& children() const;

protected:
    Node* m_parent{};
    std::array<Node*, constants::char_limit> m_children{};
};

template<std::invocable Func, typename... Args>
class FuncNode : public Node {
public:
    FuncNode(Node&& old_node, Func func, Args... args);
    void call();

private:
    Func m_func;
    std::tuple<Args...> m_args;

    using Node::m_children;
    using Node::m_parent;
};

} // namespace keybind

#include "keybind/node_inl.hpp"