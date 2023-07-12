#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace buffer {

class Node {
public:
    using Handle = std::unique_ptr<Node>;
    using View = Node*;

    Node(Handle left, Handle right);
    Node(const std::string& text);
    Node(const Node& other);

    std::size_t length() const;
    char operator[](std::size_t index) const;
    std::string substr(std::size_t start, std::size_t len) const;
    std::string to_string() const;

    friend std::pair<Handle, Handle> split(Handle node, std::size_t index);

    std::size_t depth() const;
    std::vector<View> leaves() const;

private:
    Handle m_left{};
    Handle m_right{};
    std::string m_text{};
    std::size_t m_weight{};

    bool is_leaf() const;
};

} // namespace buffer