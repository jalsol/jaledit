#pragma once

#include "keybind/trie.hpp"

#include "keybind/node.hpp"

#include "utils.hpp"

#include <ranges>

namespace keybind {

template<std::invocable Func>
void Trie::insert(std::string_view keyseq, Func func) {
    auto* current = m_root;

    for (char c : keyseq | std::views::take(keyseq.size() - 1)) {
        if (!current->child(c)) {
            current->child(c) = new Node;
        }

        current->child(c)->parent() = current;
        current = current->child(c);
    }

    current->child(keyseq.back()) = new FuncNode<Func>(func);
}

} // namespace keybind