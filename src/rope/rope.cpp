#include "rope/rope.hpp"

#include "rope/utils.hpp"

Rope::Rope() : Rope{""} {}

Rope::Rope(const std::string& text) : m_root{std::make_shared<Leaf>(text)} {}

Rope::Rope(const Rope& other) : m_root{other.m_root} {}

Rope::Rope(const Handle& root) : m_root{root} {}

std::string Rope::to_string() const { return m_root->to_string(); }

std::size_t Rope::length() const { return m_root->length(); }

char Rope::operator[](std::size_t index) const {
    return m_root->operator[](index);
}

std::string Rope::substr(std::size_t start, std::size_t length) const {
    return m_root->substr(start, length);
}

bool Rope::is_balanced() const {
    if (m_root->depth() >= Rope::max_depth - 2) {
        return false;
    }
    return m_root->length() >= fib(m_root->depth() + 2);
}

Rope Rope::rebalance() const {
    if (is_balanced()) {
        return *this;
    }

    std::vector<Node::Handle> leaves = m_root->leaves();
    return leaves_merge(leaves, 0, leaves.size());
}

Rope Rope::insert(std::size_t index, const std::string& text) const {
    return insert(index, Rope{text});
}

Rope Rope::insert(std::size_t index, const Rope& other) const {
    if (index == 0) {
        return other.append(*this);
    }

    if (index == length()) {
        return append(other);
    }

    auto [left, right] = m_root->split(index);
    return Rope{std::make_shared<Branch>(left, other.m_root)}.append(
        Rope{right});
}

Rope Rope::append(const std::string& text) const { return append(Rope{text}); }

Rope Rope::append(const Rope& other) const {
    return Rope{std::make_shared<Branch>(m_root, other.m_root)}.rebalance();
}

Rope Rope::erase(std::size_t start, std::size_t length) const {
    auto lhs = m_root->split(start);
    auto rhs = lhs.second->split(length);
    return Rope{std::make_shared<Branch>(lhs.first, rhs.second)}.rebalance();
}

Node::Handle Rope::leaves_merge(const std::vector<Node::Handle>& leaves,
                                std::size_t start, std::size_t end) {
    std::size_t range = end - start;

    if (range == 1) {
        return leaves[start];
    }

    if (range == 2) {
        return std::make_shared<Branch>(leaves[start], leaves[start + 1]);
    }

    std::size_t mid = start + (range / 2);
    return std::make_shared<Branch>(leaves_merge(leaves, start, mid),
                                    leaves_merge(leaves, mid, end));
}

Rope Rope::leaves_merge(const std::vector<Node::Handle>& leaves) {
    return Rope{leaves_merge(leaves, 0, leaves.size())};
}

Rope Rope::replace(std::size_t start, std::size_t length,
                   const std::string& text) const {
    return replace(start, length, Rope{text});
}

Rope Rope::replace(std::size_t start, std::size_t length,
                   const Rope& other) const {
    return erase(start, length).insert(start, other);
}

std::pair<Rope, Rope> Rope::split(std::size_t index) const {
    auto [left, right] = m_root->split(index);
    return {Rope{left}, Rope{right}};
}

std::size_t Rope::find_line_start(std::size_t index) const {
    return m_root->find_line_start(index);
}

std::size_t Rope::line_count() const { return m_root->lfcnt() + 1; }

Rope& Rope::operator=(const Rope& other) {
    m_root = other.m_root;
    return *this;
}

bool Rope::operator==(const Rope& other) const {
    return m_root->to_string() == other.m_root->to_string();
}

bool Rope::operator!=(const Rope& other) const {
    return m_root->to_string() != other.m_root->to_string();
}

std::ostream& operator<<(std::ostream& os, const Rope& rope) {
    return os << rope.to_string();
}