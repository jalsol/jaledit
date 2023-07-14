#include "editor.hpp"

#include "constants.hpp"
#include "raylib.h"
#include "utils.hpp"

#include <string_view>

void Editor::render() {
    Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    const int line_height = constants::font_size + 2;
    const std::size_t line_width = char_size.x;

    const auto& cursor = current_buffer().cursor();
    const auto& content = current_buffer().rope();

    const int max_line_number_size = utils::number_len(content.line_count());
    const int offset_from_number = 2;

    const int window_width = GetScreenWidth();
    const int window_height = GetScreenHeight();

    // draw cursorline
    const std::size_t cursorline_y
        = constants::margin + cursor.line * line_height;
    DrawRectangle(0, cursorline_y, GetScreenWidth(), line_height,
                  ColorAlpha(GRAY, 0.2F));

    // draw text and line numbers
    float y = constants::margin - line_height;
    int line_number = 0;
    int line_break_pos = -1;

    for (auto i = content.find_line_start(cursor.line); i < content.length();
         ++i) {
        char c = content[i];
        if (c == '\n' || line_number == 0) {
            ++line_number;
            y += line_height;

            if (y >= window_height) {
                break;
            }

            utils::draw_text(
                TextFormat("%-*d", max_line_number_size + offset_from_number,
                           line_number),
                {constants::margin, y}, BLACK, constants::font_size, 0);
        }

        if (c == '\n') {
            line_break_pos = i;
            continue;
        }

        float x
            = constants::margin
            + (max_line_number_size + offset_from_number + i - line_break_pos)
                  * char_size.x;

        if (x < window_width) {
            utils::draw_text(TextFormat("%c", c), {x, y}, BLACK,
                             constants::font_size, 0);
        }
    }

    // draw block cursor
    const float cursor_x
        = constants::margin
        + (max_line_number_size + offset_from_number + cursor.column + 1)
              * char_size.x;
    const float cursor_y = constants::margin + cursor.line * line_height;
    DrawRectangle(cursor_x, cursor_y, line_width, line_height, BLACK);
    utils::draw_text(TextFormat("%c", cursor.character), {cursor_x, cursor_y},
                     WHITE, constants::font_size, 0);
}

void Editor::open(std::string_view filename) {
    m_buffers.emplace_back(filename);
    m_buffer_id = m_buffers.size() - 1;
}

Buffer& Editor::current_buffer() { return m_buffers[m_buffer_id]; }

const Buffer& Editor::current_buffer() const { return m_buffers[m_buffer_id]; }