#include "keybind/trie.hpp"

#include "keybind/node.hpp"

#include <string_view>

namespace keybind {

Trie::Trie() { populate(); }

Trie::~Trie() { recursive_delete(m_root); }

void Trie::populate() {}

void Trie::recursive_delete(Node* node) {
    for (auto child : node->get_children()) {
        if (child) {
            recursive_delete(child);
        }
    }

    delete node;
}

} // namespace keybind