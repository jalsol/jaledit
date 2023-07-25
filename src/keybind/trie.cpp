#include "keybind/trie.hpp"

#include "keybind/node.hpp"

#include <string_view>

Keybind::Keybind() : m_current{m_root} {}

Keybind::~Keybind() { delete m_root; }

void Keybind::step(char c) {
    if (!m_current || !m_current->child(c)) {
        reset_step();
        return;
    }

    m_current = m_current->child(c);

    if (m_current->is_func()) {
        m_current->call();
        reset_step();
    }
}

void Keybind::reset_step() { m_current = m_root; }