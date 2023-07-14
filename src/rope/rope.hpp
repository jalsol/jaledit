#pragma once

#include "rope/node.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Rope {
public:
    static constexpr std::size_t max_depth = 64;

    using Handle = std::shared_ptr<Node>;

    Rope();
    Rope(const std::string& text);
    Rope(const Rope& other) = default;
    Rope(Handle root);

    std::string to_string() const;
    std::size_t length() const;
    char operator[](std::size_t index) const;
    std::string substr(std::size_t start, std::size_t length) const;

    bool is_balanced() const;
    [[nodiscard]] Rope rebalance() const;

    [[nodiscard]] Rope insert(std::size_t index, const std::string& text) const;
    [[nodiscard]] Rope insert(std::size_t index, const Rope& other) const;

    [[nodiscard]] Rope append(const std::string& text) const;
    [[nodiscard]] Rope append(const Rope& other) const;

    [[nodiscard]] Rope erase(std::size_t start, std::size_t length) const;

    [[nodiscard]] Rope replace(std::size_t start, std::size_t length,
                               const std::string& text) const;
    [[nodiscard]] Rope replace(std::size_t start, std::size_t length,
                               const Rope& other) const;

    std::pair<Rope, Rope> split(std::size_t index) const;

    std::size_t find_line_start(std::size_t index) const;
    std::size_t line_count() const;

    Rope& operator=(const Rope& other) = default;
    bool operator==(const Rope& other) const;
    bool operator!=(const Rope& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Rope& rope);

private:
    Handle m_root{};

    static Node::Handle leaves_merge(const std::vector<Node::Handle>& leaves,
                                     std::size_t start, std::size_t end);
    static Rope leaves_merge(const std::vector<Node::Handle>& leaves);
};