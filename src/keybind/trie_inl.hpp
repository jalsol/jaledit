#pragma once

#include "keybind/trie.hpp"

#include "keybind/node.hpp"

#include "utils.hpp"

namespace keybind {

template<typename Func, typename... Args>
void Trie::insert(std::string_view keyseq, Func func, Args... args) {
    auto current = m_root;

    for (auto c : keyseq) {
        if (!current->m_children[c]) {
            current->m_children[c] = new Node;
        }

        current = current->m_children[c];
    }

    Node* new_node
        = new FuncNode<Func, Args...>(std::move(*current), func, args...);
    current->get_parent()->m_children[keyseq.back()] = new_node;
    delete current;
}

} // namespace keybind