#include "editor.hpp"

#include "constants.hpp"
#include "keybind/keybind.hpp"
#include "raylib.h"
#include "utils.hpp"

#include <algorithm>
#include <climits>
#include <string_view>

constexpr int max_line_length = INT_MAX / 4 * 3;

Editor::Editor() {
    m_keybinds.insert("h", [this] { cursor_move_column(-1); });
    m_keybinds.insert("j", [this] { cursor_move_line(1); });
    m_keybinds.insert("k", [this] { cursor_move_line(-1); });
    m_keybinds.insert("l", [this] { cursor_move_column(1); });
    m_keybinds.insert("gg", [this] {
        cursor_move_line(-current_buffer().rope().line_count());
    });
    m_keybinds.insert("G", [this] {
        cursor_move_line(current_buffer().rope().line_count());
    });
    m_keybinds.insert("0", [this] { cursor_move_column(-max_line_length); });
    m_keybinds.insert("$", [this] { cursor_move_column(max_line_length); });
}

Editor::Editor(std::string_view filename) : Editor{} { open(filename); }

void Editor::render() {
    Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    const int line_height = constants::font_size + constants::line_spacing;
    const std::size_t line_width = char_size.x;

    const auto& cursor = current_buffer().cursor();
    const auto& content = current_buffer().rope();
    auto& view = current_buffer().view();

    const int max_line_number_size = utils::number_len(content.line_count());
    const int offset_from_number = 2;
    const int header_width = max_line_number_size + offset_from_number;

    view.update_header_size(header_width);

    // draw cursorline
    const int cursor_y
        = constants::margin + (cursor.line - view.offset_line()) * line_height;

    // draw block cursor
    const float cursor_x
        = constants::margin
        + (header_width + 1 + cursor.column - view.offset_column())
              * char_size.x;

    if (view.viewable_line(cursor.line, char_size)) {
        // draw cursorline
        DrawRectangle(0, cursor_y, GetScreenWidth(), line_height,
                      ColorAlpha(GRAY, 0.15F));

        // draw block cursor
        if (view.viewable_column(cursor.column, char_size)) {
            DrawRectangle(cursor_x, cursor_y, line_width, line_height,
                          ColorAlpha(ORANGE, 0.45F));
        }
    }

    // draw text and line numbers
    float y = constants::margin - line_height;
    std::size_t cur_line_idx = view.offset_line();
    std::size_t line_start = content.find_line_start(cur_line_idx);

    for (; cur_line_idx < content.line_count(); ++cur_line_idx) {
        std::size_t next_line_start = content.find_line_start(cur_line_idx + 1);

        if (view.viewable_line(cur_line_idx, char_size)) {
            y += line_height;

            std::size_t render_line_start = line_start + view.offset_column();
            std::size_t render_line_len
                = std::min(next_line_start - line_start - 1,
                           static_cast<std::size_t>(view.columns(char_size)));

            std::string line
                = content.substr(render_line_start, render_line_len);

            const char* line_text = TextFormat("%-*d%s", header_width + 1,
                                               cur_line_idx + 1, line.c_str());

            utils::draw_text(line_text, {constants::margin, y}, BLACK,
                             constants::font_size, 0);
        }

        line_start = next_line_start;
    }
}

void Editor::update() { normal_mode(); }

void Editor::open(std::string_view filename) {
    m_buffers.emplace_back(filename);
    m_buffer_id = m_buffers.size() - 1;
}

Buffer& Editor::current_buffer() { return m_buffers[m_buffer_id]; }

const Buffer& Editor::current_buffer() const { return m_buffers[m_buffer_id]; }

void Editor::normal_mode() {
    char c = GetCharPressed();

    if (c == '\0') {
        return;
    }

    m_keybinds.step(c);
}

void Editor::cursor_move_line(int delta) {
    auto& cursor = current_buffer().cursor();
    const auto& content = current_buffer().rope();
    auto& view = current_buffer().view();

    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);

    cursor.line = std::clamp(cursor.line + delta, 0,
                             static_cast<int>(content.line_count()) - 1);

    cursor_move_column(0);

    if (!view.viewable_line(cursor.line, char_size)) {
        if (delta < 0) {
            view.update_offset_line(cursor.line);
        } else if (delta > 0) {
            view.update_offset_line(cursor.line - view.lines(char_size) + 1);
        }
    }
}

void Editor::cursor_move_column(int delta) {
    auto& cursor = current_buffer().cursor();
    const auto& content = current_buffer().rope();
    auto& view = current_buffer().view();

    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    int line_length = content.line_length(cursor.line);

    cursor.column = std::clamp(cursor.column + delta, 0, line_length - 1);
    if (cursor.column < 0) {
        cursor.column = 0;
    }

    if (!view.viewable_column(cursor.column, char_size)) {
        if (delta < 0) {
            view.update_offset_column(cursor.column);
        } else if (delta > 0) {
            view.update_offset_column(cursor.column - view.columns(char_size)
                                      + 1);
        }
    }
}