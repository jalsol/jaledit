#pragma once

#include "keybind/node.hpp"

#include <concepts>
#include <string_view>

class Keybind {
public:
    Keybind();
    ~Keybind();

    template<std::invocable Func>
    void insert(std::string_view keyseq, Func func);

    void step(char c);
    void reset_step();

private:
    using Node = keybind::Node;

    Node* m_root{new Node};
    Node* m_current{};
};

#include "keybind/trie_inl.hpp"