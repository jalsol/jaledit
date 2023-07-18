#include "rope/node.hpp"

#include <cstddef>

std::size_t Node::find_line_start(std::size_t line_index) const {
    if (line_index == 0) {
        return 0;
    }
    return find_line_feed(line_index - 1) + 1;
}

std::size_t Node::length() const { return m_length; }

std::size_t Node::depth() const { return m_depth; }

std::size_t Node::lfcnt() const { return m_lfcnt; }