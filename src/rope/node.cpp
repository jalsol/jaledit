#include "rope/node.hpp"

#include <cstddef>

std::size_t Node::length() const { return m_length; }

std::size_t Node::depth() const { return m_depth; }

std::size_t Node::lfcnt() const { return m_lfcnt; }