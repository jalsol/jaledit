#pragma once

#include "rope/rope.hpp"

#include <cstddef>
#include <stack>
#include <string>
#include <string_view>

struct Cursor {
    std::size_t line{};
    std::size_t column{};
    char character{};
};

class Buffer {
public:
    Buffer();
    Buffer(std::string_view filename);

    const Cursor& cursor() const;
    const Rope& rope() const;

private:
    static constexpr std::size_t max_gap_size = 1024;

    Rope m_rope{};
    std::stack<Rope> m_undo{};
    std::stack<Rope> m_redo{};

    std::string m_gap{};
    std::size_t m_gap_start{};
    std::size_t m_gap_end{};

    Cursor m_cursor{};

    std::string_view m_filename{};
};