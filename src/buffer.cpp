#include "buffer.hpp"

#include "constants.hpp"
#include "cursor.hpp"
#include "nfd.hpp"
#include "raylib.h"
#include "rope/utils.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

Buffer::Buffer() : m_rope{"\n"} {
    m_view.update_header_size(utils::number_len(m_rope.line_count()) + 2);
}

Buffer::Buffer(std::string_view filename) : m_filename{filename} {
    std::ifstream file{filename.data()};
    if (!file) {
        throw std::runtime_error{"Could not open file"};
    }

    int fd = open(filename.data(), O_RDONLY, S_IRUSR | S_IWUSR);
    struct stat sb;

    if (fstat(fd, &sb) == -1) {
        throw std::runtime_error{"Could not get file size"};
    }

    if (sb.st_size == 0) {
        m_rope = Rope{"\n"};
        return;
    }

    char* file_in_memory = static_cast<char*>(
        mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    std::size_t remaining = sb.st_size;
    char* buf_ptr = file_in_memory;
    constexpr std::size_t buf_size = 1024 * 1024;

    while (remaining >= buf_size) {
        m_rope = m_rope.append(std::string(buf_ptr, buf_size));
        buf_ptr += buf_size;
        remaining -= buf_size;
    }
    m_rope = m_rope.append(std::string(buf_ptr, remaining));

    munmap(file_in_memory, sb.st_size);
    close(fd);

    m_view.update_header_size(utils::number_len(m_rope.line_count()) + 2);
}

Cursor& Buffer::cursor() { return m_cursor; }

const Cursor& Buffer::cursor() const { return m_cursor; }

void Buffer::set_cursor(Cursor cursor) {
    m_cursor = cursor;

    if (!m_view.viewable(m_cursor.line, m_cursor.column,
                         utils::measure_text(" ", constants::font_size, 0))) {
        m_view.update_offset_line(m_cursor.line);
        m_view.update_offset_column(m_cursor.column);
    }
}

Rope& Buffer::rope() { return m_rope; }

const Rope& Buffer::rope() const { return m_rope; }

View& Buffer::view() { return m_view; }

const View& Buffer::view() const { return m_view; }

Cursor& Buffer::select_orig() { return m_select_orig; }

const Cursor& Buffer::select_orig() const { return m_select_orig; }

const Cursor& Buffer::select_start() const {
    return m_cursor < m_select_orig ? m_cursor : m_select_orig;
}

const Cursor& Buffer::select_end() const {
    return m_cursor > m_select_orig ? m_cursor : m_select_orig;
}

Suggester& Buffer::suggester() { return m_suggester; }

const Suggester& Buffer::suggester() const { return m_suggester; }

std::string& Buffer::filename() { return m_filename; }

const std::string& Buffer::filename() const { return m_filename; }

bool Buffer::dirty() const { return m_dirty; }

void Buffer::cursor_move_line(int delta) {
    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);

    m_cursor.line = std::clamp(m_cursor.line + delta, 0,
                               static_cast<int>(m_rope.line_count()) - 1);

    cursor_move_column(0, false);

    if (!m_view.viewable_line(m_cursor.line, char_size)) {
        if (delta < 0) {
            m_view.update_offset_line(m_cursor.line);
        } else if (delta > 0) {
            m_view.update_offset_line(m_cursor.line - m_view.lines(char_size)
                                      + 1);
        }
    }
}

void Buffer::cursor_move_column(int delta, bool move_on_eol) {
    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    int line_length = m_rope.line_length(m_cursor.line);

    int right_offset
        = !move_on_eol
        + (m_cursor.line + 1 == static_cast<int>(m_rope.line_count()));

    m_cursor.column
        = std::clamp(m_cursor.column + delta, 0, line_length - right_offset);
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

    // NOTE: m_rope.length() contains a terminating null character
    // so we need to subtract 2 to get the last character

    // if already in word, move to end of word
    if (alnum_word) {
        while (index + 2 < m_rope.length() && std::isalnum(m_rope[index])) {
            cursor_move_next_char();
            ++index;
        }
    } else if (index + 2 < m_rope.length() && punct_word) {
        cursor_move_next_char();
        ++index;
        if (std::ispunct(m_rope[index])) {
            return;
        }
    }

    if (std::isspace(m_rope[index])) {
        while (index + 2 < m_rope.length() && std::isspace(m_rope[index])) {
            cursor_move_next_char();
            ++index;
        }
    }

    if (!m_view.viewable(m_cursor.line, m_cursor.column,
                         utils::measure_text(" ", constants::font_size, 0))) {
        m_view.update_offset_line(m_view.offset_line() + 1);
    }
}

void Buffer::cursor_move_prev_word() {
    if (m_cursor.line == 0 && m_cursor.column == 0) {
        return;
    }

    cursor_move_prev_char();
    std::size_t index = m_rope.index_from_pos(m_cursor.line, m_cursor.column);

    while (index > 0 && std::isspace(m_rope[index])) {
        cursor_move_prev_char();
        --index;
    }

    if (std::ispunct(m_rope[index])) {
        return;
    }

    // if already in word, move to beginning of word
    while (index > 0 && std::isalnum(m_rope[index - 1])) {
        cursor_move_prev_char();
        --index;
    }

    if (!m_view.viewable(m_cursor.line, m_cursor.column,
                         utils::measure_text(" ", constants::font_size, 0))) {
        m_view.update_offset_line(m_view.offset_line() - 1);
    }
}

void Buffer::insert_at_cursor(const std::string& text) {
    std::size_t pos = m_rope.index_from_pos(m_cursor.line, m_cursor.column);
    m_rope = m_rope.insert(pos, text);
    m_dirty = true;

    for (auto _ = text.size(); _ > 0; --_) {
        cursor_move_next_char();
    }
}

void Buffer::append_at_cursor(const std::string& text) {
    std::size_t pos = m_rope.index_from_pos(m_cursor.line, m_cursor.column);
    m_rope = m_rope.insert(pos + 1, text);
    m_dirty = true;

    for (auto _ = text.size(); _ > 0; --_) {
        cursor_move_next_char();
    }
}

void Buffer::erase_at_cursor() {
    std::size_t pos = m_rope.index_from_pos(m_cursor.line, m_cursor.column);
    if (pos == 0) {
        return;
    }

    m_undo.emplace_back(m_rope, m_cursor);
    m_redo.clear();
    cursor_move_prev_char();
    m_rope = m_rope.erase(pos - 1, 1);
    m_dirty = true;
}

void Buffer::erase_selected() {
    auto sel_start = select_start();
    auto sel_end = select_end();

    std::size_t start_idx
        = m_rope.index_from_pos(sel_start.line, sel_start.column);
    std::size_t end_idx = m_rope.index_from_pos(sel_end.line, sel_end.column);

    erase_range(start_idx, end_idx);
}

void Buffer::erase_range(std::size_t start, std::size_t end) {
    copy_range(start, end);

    m_undo.emplace_back(m_rope, select_start());
    m_redo.clear();
    m_rope = m_rope.erase(start, end - start);
    m_dirty = true;

    set_cursor(select_start());
    if (m_rope.length() == 0) {
        m_rope = m_rope.append("\n");
    }
}

void Buffer::copy_selected() {
    auto sel_start = select_start();
    auto sel_end = select_end();

    std::size_t start_idx
        = m_rope.index_from_pos(sel_start.line, sel_start.column);
    std::size_t end_idx = m_rope.index_from_pos(sel_end.line, sel_end.column);

    SetClipboardText(m_rope.substr(start_idx, end_idx - start_idx).c_str());

    set_cursor(sel_start);
}

void Buffer::copy_range(std::size_t start, std::size_t end) {
    SetClipboardText(m_rope.substr(start, end - start).c_str());
}

void Buffer::save_snapshot() {
    m_undo.push_back({m_rope, m_cursor, m_dirty});
    m_redo.clear();
}

void Buffer::undo() {
    if (m_undo.empty()) {
        return;
    }

    m_redo.push_back({m_rope, m_cursor, m_dirty});
    const auto& [prev_rope, prev_cursor, prev_dirty] = m_undo.back();
    m_rope = std::move(prev_rope);
    set_cursor(prev_cursor);
    m_dirty = prev_dirty;

    m_undo.pop_back();
}

std::optional<Rope> Buffer::undo_top() {
    if (m_undo.empty()) {
        return {};
    }

    return m_undo.back().rope;
}

void Buffer::redo() {
    if (m_redo.empty()) {
        return;
    }

    m_undo.push_back(m_redo.back());
    const auto& [next_rope, next_cursor, next_dirty] = m_redo.back();
    m_rope = std::move(next_rope);
    set_cursor(next_cursor);
    m_dirty = next_dirty;

    m_redo.pop_back();
}

void Buffer::save() {
    if (m_filename == "new file") {
        save_as();
        return;
    }

    if (!m_dirty) {
        return;
    }

    int fd = open(m_filename.data(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1) {
        throw std::runtime_error{"Could not open file"};
    }

    std::size_t text_size = m_rope.length();

    if (lseek(fd, text_size - 1, SEEK_SET) == -1) {
        close(fd);
        throw std::runtime_error{"Could not seek to end of file"};
    }

    if (write(fd, "", 1) == -1) {
        close(fd);
        throw std::runtime_error{"Could not write to file"};
    }

    char* map = static_cast<char*>(
        mmap(0, text_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    if (map == MAP_FAILED) {
        close(fd);
        throw std::runtime_error{"Could not mmap file"};
    }

    constexpr std::size_t buf_size = 1024 * 1024;
    std::size_t i;

    for (i = 0; i + buf_size < text_size; i += buf_size) {
        std::strncpy(map + i, m_rope.substr(i, buf_size).c_str(), buf_size);
    }

    std::strncpy(map + i, m_rope.substr(i, text_size - i).c_str(),
                 text_size - i);

    if (msync(map, text_size, MS_SYNC) == -1) {
        close(fd);
        munmap(map, text_size);
        throw std::runtime_error{"Could not sync file"};
    }

    if (munmap(map, text_size) == -1) {
        close(fd);
        throw std::runtime_error{"Could not unmap file"};
    }

    close(fd);
    std::cerr << "Saved " << m_filename << "\n";
    m_dirty = false;
}

void Buffer::save_as() {
    NFD::Guard nfd_guard;
    NFD::UniquePath out_path;

    nfdresult_t result
        = NFD::SaveDialog(out_path, nullptr, 0, GetWorkingDirectory());

    if (result != NFD_OKAY) {
        return;
    }

    m_filename = out_path.get();
    m_dirty = true;

    std::cerr << "Saving as " << m_filename << "\n";
    save();
}