#include "editor.hpp"

#include "constants.hpp"
#include "keybind/keybind.hpp"
#include "raylib.h"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <climits>
#include <string_view>

Editor::Editor() {
    m_keybinds.insert("h", [this] { current_buffer().cursor_move_column(-1); });
    m_keybinds.insert("j", [this] { current_buffer().cursor_move_line(1); });
    m_keybinds.insert("k", [this] { current_buffer().cursor_move_line(-1); });
    m_keybinds.insert("l", [this] { current_buffer().cursor_move_column(1); });
    m_keybinds.insert("gg", [this] {
        current_buffer().cursor_move_line(
            -current_buffer().rope().line_count());
    });
    m_keybinds.insert("G", [this] {
        current_buffer().cursor_move_line(current_buffer().rope().line_count());
    });
    m_keybinds.insert("0", [this] {
        current_buffer().cursor_move_column(-constants::max_line_length);
    });
    m_keybinds.insert("$", [this] {
        current_buffer().cursor_move_column(constants::max_line_length);
    });
    m_keybinds.insert("w",
                      [this] { current_buffer().cursor_move_next_word(); });
    m_keybinds.insert("b",
                      [this] { current_buffer().cursor_move_prev_word(); });

    m_keybinds.insert("i", [this] { set_mode(EditorMode::Insert); });
    m_keybinds.insert("o", [this] {
        current_buffer().cursor_move_column(constants::max_line_length);
        current_buffer().append_at_cursor("\n");
        current_buffer().cursor_move_line(1);
        set_mode(EditorMode::Insert);
    });
    m_keybinds.insert("O", [this] {
        current_buffer().cursor_move_column(-constants::max_line_length);
        current_buffer().insert_at_cursor("\n");
        current_buffer().cursor_move_line(-1);
        set_mode(EditorMode::Insert);
    });
    m_keybinds.insert("a", [this] {
        current_buffer().cursor_move_next_char();
        set_mode(EditorMode::Insert);
    });
    m_keybinds.insert("A", [this] {
        current_buffer().cursor_move_column(constants::max_line_length);
        current_buffer().cursor_move_next_char();
        set_mode(EditorMode::Insert);
    });
    m_keybinds.insert("u", [this] { current_buffer().undo(); });
    m_keybinds.insert("r", [this] { current_buffer().redo(); });
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
            std::size_t render_line_len = std::min(
                next_line_start - line_start - 1,
                static_cast<std::size_t>(view.columns(char_size) - 1));

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

int shift(int key) {
    if (std::isalpha(key)) {
        return std::toupper(key);
    }

    constexpr std::array<std::pair<int, int>, 21> map = {{
        {',', '<'}, {'.', '>'}, {'/', '?'},  {';', ':'}, {'\'', '"'},
        {'[', '{'}, {']', '}'}, {'\\', '|'}, {'`', '~'}, {'-', '_'},
        {'=', '+'}, {'1', '!'}, {'2', '@'},  {'3', '#'}, {'4', '$'},
        {'5', '%'}, {'6', '^'}, {'7', '&'},  {'8', '*'}, {'9', '('},
        {'0', ')'},
    }};

    for (const auto& [from, to] : map) {
        if (from == key) {
            return to;
        }
    }

    return key;
}

Key modify_key(int key, int prev_key) {
    switch (key) {
    case KEY_ENTER:
        key = '\n';
        break;
    }

    switch (prev_key) {
    case KEY_LEFT_SHIFT:
    case KEY_RIGHT_SHIFT:
        return {KEY_NULL, shift(key)};

    case KEY_LEFT_CONTROL:
    case KEY_RIGHT_CONTROL:
        return {KEY_LEFT_CONTROL, key};

    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:
        return {KEY_LEFT_ALT, key};

    default:
        return {KEY_NULL, std::tolower(key)};
    }
}

void Editor::update() {
    static int prev_key = KEY_NULL;
    Key rv;
    int key = GetKeyPressed();

    if (key == KEY_NULL) {
        if (IsKeyUp(prev_key)) {
            prev_key = KEY_NULL;
        }

        return;
    }

    if ((KEY_APOSTROPHE <= key && key <= KEY_GRAVE) || key == KEY_ENTER) {
        rv = modify_key(key, prev_key);
    } else if (KEY_LEFT_SHIFT <= key && key <= KEY_RIGHT_SUPER) {
        prev_key = key;
        return;
    } else {
        rv = {KEY_NULL, key};
    }

    switch (m_mode) {
    case EditorMode::Normal:
        normal_mode(rv);
        break;
    case EditorMode::Insert:
        insert_mode(rv);
        break;
    default:
        break;
    }
}

void Editor::open(std::string_view filename) {
    if (filename.empty()) {
        m_buffers.emplace_back();
    } else {
        m_buffers.emplace_back(filename);
    }

    m_buffer_id = m_buffers.size() - 1;
}

Buffer& Editor::current_buffer() { return m_buffers[m_buffer_id]; }

const Buffer& Editor::current_buffer() const { return m_buffers[m_buffer_id]; }

void Editor::set_mode(EditorMode mode) { m_mode = mode; }

void Editor::normal_mode(Key key) {
    if (key.modifier != KEY_NULL) {
        m_keybinds.reset_step();
    } else {
        m_keybinds.step(key.key);
    }
}

void Editor::insert_mode(Key key) {
    if (key.key == KEY_ESCAPE) {
        set_mode(EditorMode::Normal);
        return;
    }

    if (key.key == KEY_BACKSPACE) {
        current_buffer().erase_at_cursor();
        return;
    }

    if (key.key == KEY_TAB) {
        current_buffer().insert_at_cursor(std::string(4, ' '));
        return;
    }

    current_buffer().insert_at_cursor(std::string{char(key.key)});
}

void Editor::undo() { current_buffer().undo(); }

void Editor::redo() { current_buffer().redo(); }
