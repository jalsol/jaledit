#include "buffer.hpp"

#include "constants.hpp"
#include "raylib.h"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string_view>

int View::lines(Vector2 char_size) {
    return static_cast<int>(GetScreenHeight() - constants::margin)
         / (char_size.y + constants::line_spacing);
}

int View::columns(Vector2 char_size) const {
    return static_cast<int>(GetScreenWidth() - constants::margin - 1)
             / char_size.x
         - m_header_size - 1;
}

int View::offset_line() const { return m_offset_line; }

int View::offset_column() const { return m_offset_column; }

void View::update_offset_line(int line) { m_offset_line = line; }

void View::update_offset_column(int column) { m_offset_column = column; }

void View::update_header_size(int size) { m_header_size = size; }

bool View::viewable(int line, int column, Vector2 char_size) const {
    return viewable_line(line, char_size) && viewable_column(column, char_size);
}

bool View::viewable_line(int line, Vector2 char_size) const {
    return line >= m_offset_line && line < m_offset_line + lines(char_size);
}

bool View::viewable_column(int column, Vector2 char_size) const {
    return column >= m_offset_column
        && column < m_offset_column + columns(char_size);
}

Buffer::Buffer() : m_rope{""} {
    m_view.update_header_size(utils::number_len(m_rope.line_count()) + 2);
}

Buffer::Buffer(std::string_view filename) : m_filename{filename} {
    std::ifstream file{filename.data()};
    if (!file) {
        throw std::runtime_error{"Could not open file"};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_rope = Rope{buffer.str()};
    m_view.update_header_size(utils::number_len(m_rope.line_count()) + 2);
}

Cursor& Buffer::cursor() { return m_cursor; }

const Cursor& Buffer::cursor() const { return m_cursor; }

const Rope& Buffer::rope() const { return m_rope; }

View& Buffer::view() { return m_view; }

const View& Buffer::view() const { return m_view; }

void Buffer::cursor_move_line(int delta) {
    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);

    m_cursor.line = std::clamp(m_cursor.line + delta, 0,
                               static_cast<int>(m_rope.line_count()) - 1);

    cursor_move_column(0);

    if (!m_view.viewable_line(m_cursor.line, char_size)) {
        if (delta < 0) {
            m_view.update_offset_line(m_cursor.line);
        } else if (delta > 0) {
            m_view.update_offset_line(m_cursor.line - m_view.lines(char_size)
                                      + 1);
        }
    }
}

void Buffer::cursor_move_column(int delta) {
    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    int line_length = m_rope.line_length(m_cursor.line);

    m_cursor.column = std::clamp(m_cursor.column + delta, 0, line_length - 1);
    if (m_cursor.column < 0) {
        m_cursor.column = 0;
    }

    if (!m_view.viewable_column(m_cursor.column, char_size)) {
        if (delta < 0) {
            m_view.update_offset_column(m_cursor.column);
        } else if (delta > 0) {
            m_view.update_offset_column(m_cursor.column
                                        - m_view.columns(char_size) + 1);
        }
    }
}

void Buffer::cursor_move_next_char() {
    if (m_cursor.column
        == static_cast<int>(m_rope.line_length(m_cursor.line))) {
        ++m_cursor.line;
        m_cursor.column = 0;
    } else {
        ++m_cursor.column;
    }
}

void Buffer::cursor_move_prev_char() {
    if (m_cursor.column == 0) {
        --m_cursor.line;
        m_cursor.column = m_rope.line_length(m_cursor.line);
    } else {
        --m_cursor.column;
    }
}

void Buffer::cursor_move_next_word() {
    std::size_t index = m_rope.index_from_pos(m_cursor.line, m_cursor.column);
    char c = m_rope[index];
    bool alnum_word = !!std::isalnum(c);
    bool punct_word = !!std::ispunct(c);

    // if already in word, move to end of word
    if (alnum_word) {
        while (std::isalnum(m_rope[index])) {
            cursor_move_next_char();
            ++index;
        }
    } else if (punct_word) {
        cursor_move_next_char();
        ++index;
        if (std::ispunct(m_rope[index])) {
            return;
        }
    }

    if (std::isspace(m_rope[index])) {
        while (std::isspace(m_rope[index])) {
            cursor_move_next_char();
            ++index;
        }
    }
}

void Buffer::cursor_move_prev_word() {
    cursor_move_prev_char();
    std::size_t index = m_rope.index_from_pos(m_cursor.line, m_cursor.column);

    while (std::isspace(m_rope[index])) {
        cursor_move_prev_char();
        --index;
    }

    if (std::ispunct(m_rope[index])) {
        return;
    }

    // if already in word, move to beginning of word
    while (std::isalnum(m_rope[index - 1])) {
        cursor_move_prev_char();
        --index;
    }
}

void Buffer::insert_at_cursor(const std::string& text) {
    std::size_t pos = m_rope.index_from_pos(m_cursor.line, m_cursor.column);
    m_undo.push(m_rope);
    m_rope = m_rope.insert(pos, text);
    cursor_move_next_char();
}

void Buffer::erase_at_cursor() {
    std::size_t pos = m_rope.index_from_pos(m_cursor.line, m_cursor.column);
    if (pos == 0) {
        return;
    }

    m_undo.push(m_rope);
    cursor_move_prev_char();
    m_rope = m_rope.erase(pos - 1, 1);
}