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
    virtual std::size_t find_line_feed(std::size_t index) const = 0;
    virtual ~Node() = default;

    std::size_t find_line_start(std::size_t line_index) const;
    std::size_t length() const;
    std::size_t depth() const;
    std::size_t lfcnt() const;

protected:
    std::size_t m_depth{};

    std::size_t m_length{};
    std::size_t m_weight{};

    std::size_t m_lfcnt;
    std::size_t m_lfweight{};
};

class Leaf : public Node {
public:
    Leaf(const std::string& text);
    ~Leaf() override = default;

    char operator[](std::size_t index) const override;
    std::string substr(std::size_t start, std::size_t length) const override;
    std::string to_string() const override;
    std::pair<Node::Handle, Node::Handle>
    split(std::size_t index) const override;
    std::vector<Node::Handle> leaves() const override;
    std::size_t find_line_feed(std::size_t index) const override;

private:
    using Node::m_depth;

    using Node::m_length;
    using Node::m_weight;

    using Node::m_lfcnt;
    using Node::m_lfweight;

    std::string m_text{};
    std::vector<std::size_t> m_lfpos{};
};

class Branch : public Node {
public:
    Branch(Handle left, Handle right);
    Branch(const Branch& other);
    ~Branch() override = default;

    char operator[](std::size_t index) const override;
    std::string substr(std::size_t start, std::size_t length) const override;
    std::string to_string() const override;
    std::pair<Node::Handle, Node::Handle>
    split(std::size_t index) const override;
    std::vector<Node::Handle> leaves() const override;
    std::size_t find_line_feed(std::size_t index) const override;

private:
    using Node::m_depth;

    using Node::m_length;
    using Node::m_weight;

    using Node::m_lfcnt;
    using Node::m_lfweight;

    Node::Handle m_left{};
    Node::Handle m_right{};
};