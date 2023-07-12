#include "buffer/node.hpp"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace buffer {

std::invalid_argument ERROR_OOB_NODE
    = std::invalid_argument("Error: string index out of bounds");

Node::Node(Handle left, Handle right)
    : m_left{std::move(left)},
      m_right{std::move(right)},
      m_weight{left->length()} {}

Node::Node(const std::string& text) : m_text{text}, m_weight{text.length()} {}

Node::Node(const Node& other) : m_weight{other.m_weight}, m_text{other.m_text} {
    View other_left = other.m_left.get();
    View other_right = other.m_right.get();

    m_left = other_left ? std::make_unique<Node>(*other_left) : nullptr;
    m_right = other_right ? std::make_unique<Node>(*other_right) : nullptr;
}

std::size_t Node::length() const {
    std::size_t result = m_weight;

    if (!is_leaf()) {
        result += m_right ? m_right->length() : 0;
    }

    return result;
}

char Node::operator[](std::size_t index) const {
    if (is_leaf()) {
        if (index >= m_weight) {
            throw ERROR_OOB_NODE;
        }

        return m_text[index];
    }

    if (index < m_weight) {
        return m_left->operator[](index);
    } else {
        return m_right->operator[](index - m_weight);
    }
}

std::string Node::substr(std::size_t start, std::size_t len) const {
    if (is_leaf()) {
        return (len < m_weight) ? m_text.substr(start, len) : m_text;
    }

    if (start >= m_weight) {
        return m_right ? m_right->substr(start - m_weight, len) : "";
    }

    std::string result = m_left ? m_left->substr(start, len) : "";

    if (start + len > m_weight) {
        std::size_t remaining = m_weight - start;
        std::string right_result
            = m_right ? m_right->substr(m_weight, len - remaining) : "";
        result += right_result;
    }

    return result;
}

std::string Node::to_string() const {
    if (is_leaf()) {
        return m_text;
    }

    std::string result = m_left ? m_left->to_string() : "";

    if (m_right) {
        result += m_right->to_string();
    }

    return result;
}

bool Node::is_leaf() const { return m_left == nullptr && m_right == nullptr; }

std::pair<Node::Handle, Node::Handle> split(Node::Handle node,
                                            std::size_t index) {
    std::size_t weight = node->m_weight;

    if (node->is_leaf()) {
        return {
            std::make_unique<Node>(node->substr(0, index)),
            std::make_unique<Node>(node->substr(index, weight - index)),
        };
    }

    if (index == weight) {
        return {std::move(node->m_left), std::move(node->m_right)};
    }

    Node::Handle old_right = std::move(node->m_right);

    if (index < weight) {
        node->m_right = nullptr;
        node->m_weight = index;

        auto [lsplit_left, lsplit_right]
            = split(std::move(node->m_left), index);
        node->m_left = std::move(lsplit_left);

        return {
            std::move(node),
            make_unique<Node>(std::move(lsplit_right), std::move(old_right)),
        };
    } else {
        auto [rsplit_left, rsplit_right]
            = split(std::move(old_right), index - weight);
        node->m_right = std::move(rsplit_left);
        return {
            std::move(node),
            std::move(rsplit_right),
        };
    }
}

std::size_t Node::depth() const {
    if (is_leaf()) {
        return 0;
    }

    std::size_t left_depth = m_left ? m_left->depth() : 0;
    std::size_t right_depth = m_right ? m_right->depth() : 0;
    return 1 + std::max(left_depth, right_depth);
}

std::vector<Node::View> Node::leaves() const {
    if (is_leaf()) {
        return {const_cast<Node*>(this)};
    }

    std::vector<View> result;

    if (m_left) {
        auto left_leaves = m_left->leaves();
        result.insert(result.end(), left_leaves.begin(), left_leaves.end());
    }

    if (m_right) {
        auto right_leaves = m_right->leaves();
        result.insert(result.end(), right_leaves.begin(), right_leaves.end());
    }

    return result;
}

} // namespace buffer