#pragma once

#include "keybind/node.hpp"

#include <string_view>

namespace keybind {

class Trie {
public:
    Trie();
    ~Trie();

private:
    Node* m_root{};
    Node* m_current{};

    void populate();

    template<typename Func, typename... Args>
    void insert(std::string_view keyseq, Func func, Args... args);

    void recursive_delete(Node* node);
};

} // namespace keybind

#include "keybind/trie_inl.hpp"