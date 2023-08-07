#pragma once

#include "keybind/node.hpp"

#include <concepts>
#include <functional>
#include <iostream>
#include <memory>

namespace keybind {

template<std::invocable Func>
FuncNode<Func>::FuncNode(Func func, bool editable) : m_func{func} {
    m_editable = editable;

    for (auto* child : m_children) {
        if (child != nullptr) {
            child->parent() = this;
        }
    }
}

template<std::invocable Func>
void FuncNode<Func>::call(bool editable) {
    if (m_editable && !editable) {
        std::cout << "Not editable" << std::endl;
    } else {
        std::invoke(m_func);
    }
}

} // namespace keybind
