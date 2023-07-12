#pragma once

#include <memory>

namespace keybind {

template<typename Func, typename... Args>
FuncNode<Func, Args...>::FuncNode(Node&& old_node, Func func, Args... args)
    : m_func{func}, m_args{args...} {
    m_parent = old_node.m_parent;
    m_children = std::move(old_node.m_children);

    for (auto child : m_children) {
        if (child) {
            child->m_parent = this;
        }
    }
}

template<typename Func, typename... Args>
void FuncNode<Func, Args...>::call() {
    std::apply(m_func, m_args);
}

} // namespace keybind
