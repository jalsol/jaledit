#pragma once

#include "keybind/trie.hpp"

#include "keybind/node.hpp"

#include "utils.hpp"

#include <ranges>

template<std::invocable Func>
void Keybind::insert(std::string_view keyseq, Func func, bool editable) {
    auto* current = m_root;

    for (char c : keyseq | std::views::take(keyseq.size() - 1)) {
        if (!current->child(c)) {
            current->child(c) = new keybind::Node;
        }

        current->child(c)->parent() = current;
        current = current->child(c);
    }

    current->child(keyseq.back()) = new keybind::FuncNode<Func>(func, editable);
}