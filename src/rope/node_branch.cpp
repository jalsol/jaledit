#include "rope/node.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

Branch::Branch(const Handle& left, const Handle& right)
    : m_left{left}, m_right{right} {
    m_weight = m_left->length();
    m_length = m_weight + (m_right ? m_right->length() : 0);

    std::size_t left_depth = m_left ? m_left->depth() : 0;
    std::size_t right_depth = m_right ? m_right->depth() : 0;
    m_depth = std::max(left_depth, right_depth) + 1;
}

Branch::Branch(const Branch& other)
    : m_left{other.m_left}, m_right{other.m_right} {
    m_weight = m_left->length();
}

char Branch::operator[](std::size_t index) const {
    if (index < m_weight) {
        return m_left->operator[](index);
    } else {
        return m_right->operator[](index - m_weight);
    }
}

std::string Branch::substr(std::size_t start, std::size_t length) const {
    if (start >= m_weight) {
        return m_right ? m_right->substr(start - m_weight, length) : "";
    }

    std::string result = m_left ? m_left->substr(start, length) : "";

    if (start + length > m_weight) {
        std::size_t offset = m_weight - start;
        result += m_right ? m_right->substr(m_weight, length - offset) : "";
    }

    return result;
}

std::string Branch::to_string() const {
    std::string result = m_left ? m_left->to_string() : "";
    result += m_right ? m_right->to_string() : "";
    return result;
}

std::pair<Node::Handle, Node::Handle> Branch::split(std::size_t index) const {
    if (index == m_weight) {
        return {m_left, m_right};
    }

    if (index < m_weight) {
        const auto& [lsplit_left, lsplit_right]
            = m_left ? m_left->split(index) : std::pair{nullptr, nullptr};

        return {
            lsplit_left,
            std::make_shared<Branch>(lsplit_right, m_right),
        };
    } else {
        const auto& [rsplit_left, rsplit_right]
            = m_right ? m_right->split(index - m_weight)
                      : std::pair{nullptr, nullptr};

        return {
            std::make_shared<Branch>(m_left, rsplit_left),
            rsplit_right,
        };
    }
}

std::vector<Node::Handle> Branch::leaves() const {
    std::vector<Node::Handle> result;

    if (m_left) {
        auto left = m_left->leaves();
        result.insert(result.end(), left.begin(), left.end());
    }

    if (m_right) {
        auto right = m_right->leaves();
        result.insert(result.end(), right.begin(), right.end());
    }

    return result;
}