#pragma once

#include "keybind/node.hpp"

#include <concepts>
#include <string_view>

namespace keybind {

class Trie {
public:
    Trie();
    ~Trie();

    template<std::invocable Func>
    void insert(std::string_view keyseq, Func func);

    void step(char c);
    void reset_step();

private:
    Node* m_root{new Node};
    Node* m_current{};
};

} // namespace keybind

#include "keybind/trie_inl.hpp"