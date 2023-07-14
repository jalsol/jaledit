#pragma once

#include "keybind/node.hpp"

#include <concepts>
#include <memory>

namespace keybind {

template<std::invocable Func, typename... Args>
FuncNode<Func, Args...>::FuncNode(Node&& old_node, Func func, Args... args)
    : m_func{func}, m_args{args...} {
    m_parent = old_node.parent();
    m_children = std::move(old_node.children());

    for (auto* child : m_children) {
        if (child != nullptr) {
            child->m_parent = this;
        }
    }
}

template<std::invocable Func, typename... Args>
void FuncNode<Func, Args...>::call() {
    std::apply(m_func, m_args);
}

} // namespace keybind
