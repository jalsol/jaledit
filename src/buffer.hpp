#pragma once

#include "rope/rope.hpp"

#include "raylib.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct Cursor {
    int line{};
    int column{};

    auto operator<=>(const Cursor&) const = default;
};

class View {
public:
    static int lines(Vector2 char_size);
    int columns(Vector2 char_size) const;

    int offset_line() const;
    int offset_column() const;
    void update_offset_line(int line);
    void update_offset_column(int column);
    void update_header_size(int size);

    bool viewable(int line, int column, Vector2 char_size) const;
    bool viewable_line(int line, Vector2 char_size) const;
    bool viewable_column(int column, Vector2 char_size) const;

private:
    int m_offset_line{};
    int m_offset_column{};
    int m_header_size{};
};

class Buffer {
public:
    Buffer();
    Buffer(std::string_view filename);

    Cursor& cursor();
    const Cursor& cursor() const;
    void set_cursor(Cursor cursor);

    const Rope& rope() const;

    View& view();
    const View& view() const;

    Cursor& select_orig();
    const Cursor& select_orig() const;
    const Cursor& select_start() const;
    const Cursor& select_end() const;

    std::string_view filename() const;

    bool dirty() const;

    void cursor_move_line(int delta);
    void cursor_move_column(int delta, bool move_on_eol);
    void cursor_move_next_char();
    void cursor_move_prev_char();
    void cursor_move_next_word();
    void cursor_move_prev_word();

    void insert_at_cursor(const std::string& text);
    void append_at_cursor(const std::string& text);
    void erase_at_cursor();
    void erase_selected();
    void erase_range(std::size_t start, std::size_t end);
    void copy_selected();
    void copy_range(std::size_t start, std::size_t end);
    void undo();
    void redo();
    void save();

private:
    // The value of 32 is small enough for demo purposes,
    // but in a real editor this value would be much larger.
    // static constexpr std::size_t max_gap_size = 32;

    Rope m_rope{};
    std::vector<std::pair<Rope, Cursor>> m_undo{};
    std::vector<std::pair<Rope, Cursor>> m_redo{};

    // std::string m_gap{};
    // std::size_t m_gap_start{};

    Cursor m_cursor{};
    View m_view{};

    Cursor m_select_orig{-1, -1};

    std::string_view m_filename{};

    bool m_dirty{};
};