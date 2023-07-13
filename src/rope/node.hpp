#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Node {
public:
    using Handle = std::shared_ptr<Node>;

    virtual char operator[](std::size_t index) const = 0;
    virtual std::string substr(std::size_t start, std::size_t length) const = 0;
    virtual std::string to_string() const = 0;
    virtual std::pair<Handle, Handle> split(std::size_t index) const = 0;
    virtual std::vector<Handle> leaves() const = 0;

    std::size_t length() const;
    std::size_t depth() const;

protected:
    std::size_t m_weight{};
    std::size_t m_length{};
    std::size_t m_depth{};
};

class Leaf : public Node {
public:
    Leaf(const std::string& text);

    char operator[](std::size_t index) const override;
    std::string substr(std::size_t start, std::size_t length) const override;
    std::string to_string() const override;
    std::pair<Node::Handle, Node::Handle>
    split(std::size_t index) const override;
    std::vector<Node::Handle> leaves() const override;

private:
    using Node::m_depth;
    using Node::m_length;
    using Node::m_weight;
    std::string m_text{};
};

class Branch : public Node {
public:
    Branch(const Handle& left, const Handle& right);
    Branch(const Branch& other);

    char operator[](std::size_t index) const override;
    std::string substr(std::size_t start, std::size_t length) const override;
    std::string to_string() const override;
    std::pair<Node::Handle, Node::Handle>
    split(std::size_t index) const override;
    std::vector<Node::Handle> leaves() const override;

private:
    using Node::m_depth;
    using Node::m_length;
    using Node::m_weight;
    Node::Handle m_left{};
    Node::Handle m_right{};
};