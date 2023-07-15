#include "keybind/trie.hpp"

#include "keybind/node.hpp"

#include <string_view>

namespace keybind {

Trie::Trie() : m_current{m_root} {}

Trie::~Trie() { delete m_root; }

void Trie::step(char c) {
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

void Trie::reset_step() { m_current = m_root; }

} // namespace keybind