#pragma once

#include "keybind/node.hpp"

#include <concepts>
#include <functional>
#include <iostream>
#include <memory>

namespace keybind {

template<std::invocable Func>
FuncNode<Func>::FuncNode(Func func) : m_func{func} {
    for (auto* child : m_children) {
        if (child != nullptr) {
            child->parent() = this;
        }
    }
}

template<std::invocable Func>
void FuncNode<Func>::call() {
    std::invoke(m_func);
}

} // namespace keybind
