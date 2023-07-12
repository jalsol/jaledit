#pragma once

#include "constants.hpp"
#include "utils.hpp"

#include <array>
#include <tuple>

namespace keybind {

class Node {
public:
    void set_parent(Node* parent);
    Node* get_parent() const;
    std::array<Node*, constants::char_limit>& get_children();
    std::array<Node*, constants::char_limit> get_children() const;

protected:
    Node* m_parent{};
    std::array<Node*, constants::char_limit> m_children{};
};

template<typename Func, typename... Args>
class FuncNode : public Node {
public:
    FuncNode(Node&& old_node, Func func, Args... args);
    void call();

private:
    Func m_func;
    std::tuple<Args...> m_args;
};

} // namespace keybind

#include "keybind/node_inl.hpp"