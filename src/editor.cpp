#include "editor.hpp"

#include "constants.hpp"
#include "highlight/highlight.hpp"
#include "keybind/keybind.hpp"
#include "nfd.hpp"
#include "raylib.h"
#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <climits>
#include <string_view>

Editor::Editor() {
    m_keybinds.insert(
        "h",
        [this] {
            current_buffer().cursor_move_column(-1,
                                                m_mode == EditorMode::Visual);
        },
        false);
    m_keybinds.insert(
        "j", [this] { current_buffer().cursor_move_line(1); }, false);
    m_keybinds.insert(
        "k", [this] { current_buffer().cursor_move_line(-1); }, false);
    m_keybinds.insert(
        "l",
        [this] {
            current_buffer().cursor_move_column(1,
                                                m_mode == EditorMode::Visual);
        },
        false);
    m_keybinds.insert(
        "gg",
        [this] {
            current_buffer().cursor_move_line(
                -current_buffer().rope().line_count());
        },
        false);
    m_keybinds.insert(
        "G",
        [this] {
            current_buffer().cursor_move_line(
                current_buffer().rope().line_count());
        },
        false);
    m_keybinds.insert(
        "0",
        [this] {
            current_buffer().cursor_move_column(-constants::max_line_length,
                                                m_mode == EditorMode::Visual);
        },
        false);
    m_keybinds.insert(
        "$",
        [this] {
            current_buffer().cursor_move_column(constants::max_line_length,
                                                m_mode == EditorMode::Visual);
        },
        false);
    m_keybinds.insert(
        "w", [this] { current_buffer().cursor_move_next_word(); }, false);
    m_keybinds.insert(
        "b", [this] { current_buffer().cursor_move_prev_word(); }, false);

    m_keybinds.insert(
        "i", [this] { set_mode(EditorMode::Insert); }, true);
    m_keybinds.insert(
        "v",
        [this] {
            current_buffer().select_orig() = current_buffer().cursor();
            set_mode(EditorMode::Visual);
        },
        false);
    m_keybinds.insert(
        "o",
        [this] {
            set_mode(EditorMode::Insert);
            current_buffer().cursor_move_column(constants::max_line_length,
                                                m_mode == EditorMode::Visual);
            current_buffer().append_at_cursor("\n");
            if (current_buffer().rope().line_length(
                    current_buffer().cursor().line)
                > 0) {
                current_buffer().cursor_move_line(1);
            }
        },
        true);
    m_keybinds.insert(
        "O",
        [this] {
            set_mode(EditorMode::Insert);
            current_buffer().cursor_move_column(-constants::max_line_length,
                                                m_mode == EditorMode::Visual);
            current_buffer().insert_at_cursor("\n");
            current_buffer().cursor_move_line(-1);
        },
        true);
    m_keybinds.insert(
        "a",
        [this] {
            current_buffer().cursor_move_next_char();
            set_mode(EditorMode::Insert);
        },
        true);
    m_keybinds.insert(
        "A",
        [this] {
            current_buffer().cursor_move_column(constants::max_line_length,
                                                m_mode == EditorMode::Visual);
            current_buffer().cursor_move_next_char();
            set_mode(EditorMode::Insert);
        },
        true);
    m_keybinds.insert(
        "u", [this] { current_buffer().undo(); }, true);
    m_keybinds.insert(
        "r", [this] { current_buffer().redo(); }, true);
    m_keybinds.insert(
        "x",
        [this] {
            auto& buffer = current_buffer();
            const auto& cursor = buffer.cursor();
            const auto& rope = buffer.rope();

            char cur_char
                = rope[rope.index_from_pos(cursor.line, cursor.column)];
            if (cur_char == '\n' || cur_char == '\0') {
                return;
            }

            buffer.cursor_move_next_char();
            buffer.erase_at_cursor();

            cur_char = rope[rope.index_from_pos(cursor.line, cursor.column)];
            if ((cur_char == '\n' || cur_char == '\0') && cursor.column > 0) {
                buffer.cursor_move_prev_char();
            }
        },
        true);
    m_keybinds.insert(
        "p",
        [this] {
            auto& buffer = current_buffer();
            auto cursor = buffer.cursor();
            const auto& rope = buffer.rope();

            buffer.save_snapshot();

            if (rope[rope.index_from_pos(cursor.line, cursor.column)] == '\n') {
                buffer.insert_at_cursor(GetClipboardText());
                buffer.cursor_move_column(-1, false);
            } else {
                buffer.append_at_cursor(GetClipboardText());
            }

            buffer.set_cursor(cursor);
        },
        true);
    m_keybinds.insert(
        "dd",
        [this] {
            auto& buffer = current_buffer();
            const auto& cursor = buffer.cursor();
            const auto& rope = buffer.rope();

            buffer.save_snapshot();

            int line_start = rope.find_line_start(cursor.line);
            int next_line_start = rope.find_line_start(cursor.line + 1);
            current_buffer().select_orig()
                = {cursor.line, next_line_start - line_start};
            current_buffer().cursor_move_column(-constants::max_line_length,
                                                false);
            current_buffer().erase_range(line_start, next_line_start);
        },
        true);
    m_keybinds.insert(
        "yy",
        [this] {
            auto& buffer = current_buffer();
            const auto& cursor = buffer.cursor();
            const auto& rope = buffer.rope();

            buffer.save_snapshot();

            std::size_t line_start = rope.find_line_start(cursor.line);
            std::size_t line_end = rope.find_line_start(cursor.line + 1) - 1;
            current_buffer().copy_range(line_start, line_end);
        },
        true);
    m_keybinds.insert(
        "dw",
        [this] {
            auto& buffer = current_buffer();
            auto cursor = buffer.cursor();

            buffer.save_snapshot();

            current_buffer().select_orig() = cursor;
            current_buffer().cursor_move_next_word();
            current_buffer().erase_selected();
        },
        true);
    m_keybinds.insert(
        "cw",
        [this] {
            auto& buffer = current_buffer();
            auto cursor = buffer.cursor();

            buffer.save_snapshot();

            current_buffer().select_orig() = cursor;
            current_buffer().cursor_move_next_word();
            current_buffer().erase_selected();

            set_mode(EditorMode::Insert);
        },
        true);
    m_keybinds.insert(
        "s", [this] { current_buffer().save(); }, true);
    m_keybinds.insert(
        "S", [this] { current_buffer().save_as(); }, true);
    m_keybinds.insert(
        "F", [this] { open_file_dialog(); }, false);
    m_keybinds.insert(
        "f", [this] { set_mode(EditorMode::BufferList); }, false);

    m_keybinds.insert(
        "/",
        [this] {
            set_mode(EditorMode::Finder);
            m_finder.toggle_prompt(FinderMode::Find);
        },
        false);
    m_keybinds.insert(
        "?",
        [this] {
            set_mode(EditorMode::Finder);
            m_finder.toggle_prompt(FinderMode::Replace);
        },
        false);
    m_keybinds.insert(
        "n",
        [this] {
            m_finder.set_to_highlight(true);
            current_buffer().cursor()
                = m_finder.next_match(current_buffer().cursor());
        },
        false);
    m_keybinds.insert(
        "N",
        [this] {
            m_finder.set_to_highlight(true);
            current_buffer().cursor()
                = m_finder.prev_match(current_buffer().cursor());
        },
        false);
}

Editor::Editor(std::string_view filename) : Editor{} { open(filename); }

void Editor::render_status_bar() {
    // draw status
    std::string_view status;

    switch (m_mode) {
    case EditorMode::Normal:
        status = "NORMAL";
        break;
    case EditorMode::Insert:
        status = "INSERT";
        break;
    case EditorMode::Visual:
        status = "VISUAL";
        break;
    case EditorMode::BufferList:
        status = "BUFFER LIST";
        break;
    case EditorMode::Finder:
        status = "FINDER";
        break;
    default:
        utils::unreachable();
    }

    utils::draw_text(status.data(), {constants::margin, 0}, BLACK,
                     constants::font_size, 0);

    // draw filename
    if (m_mode != EditorMode::BufferList) {
        std::string_view filename = current_buffer().filename();

        float filename_width
            = utils::measure_text(filename.data(), constants::font_size, 0).x;

        utils::draw_text(
            filename.data(),
            {GetScreenWidth() - constants::margin - filename_width, 0}, BLACK,
            constants::font_size, 0);

        if (current_buffer().dirty()) {
            utils::draw_text("*",
                             {(float)GetScreenWidth() - constants::margin, 0},
                             BLACK, constants::font_size, 0);
        }
    }
    // draw status background
    DrawRectangle(0, 0, GetScreenWidth(), constants::margin,
                  ColorAlpha(ORANGE, 0.2F));
}

void Editor::render_buffer() {
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

    if (!view.viewable_line(cursor.line, char_size)) {
        view.update_offset_line(cursor.line);

        if (!view.viewable_column(cursor.column, char_size)) {
            view.update_offset_column(cursor.column);
        }
    }

    // draw text and line numbers
    float y = constants::margin - line_height;
    std::size_t cur_line_idx = view.offset_line();
    std::size_t line_start = content.find_line_start(cur_line_idx);
    std::size_t next_line_start;

    for (; cur_line_idx < content.line_count();
         ++cur_line_idx, line_start = next_line_start) {
        next_line_start = content.find_line_start(cur_line_idx + 1);

        if (!view.viewable_line(cur_line_idx, char_size)) {
            break;
        }

        y += line_height;

        std::size_t render_line_start = line_start + view.offset_column();
        std::size_t line_len = next_line_start - line_start;

        if (cur_line_idx + 1 == content.line_count()) {
            ++line_len;
        }

        std::size_t render_line_len
            = std::min(line_len - 1,
                       static_cast<std::size_t>(view.columns(char_size) - 1));

        std::string line = content.substr(render_line_start, render_line_len);

        const char* line_number
            = TextFormat("%-*d", header_width + 1, cur_line_idx + 1);

        utils::draw_text(line_number, {constants::margin, y}, BLACK,
                         constants::font_size, 0);

        float x = constants::margin + (header_width + 1) * char_size.x;
        Highlighter highlighter(line);
        HighlightedToken token;

        while (true) {
            token = highlighter.next();

            if (token.token.kind() == TokenKind::End) {
                break;
            }

            std::string_view text = token.token.text() == "\t"
                                      ? std::string(8, ' ')
                                      : token.token.text();

            utils::draw_text(text.data(), {x, y}, token.color,
                             constants::font_size, 0);

            x += text.size() * char_size.x;
        }

        if (m_mode == EditorMode::Visual) {
            const auto& select_start = current_buffer().select_start();
            const auto& select_end = current_buffer().select_end();

            std::size_t select_start_idx = content.index_from_pos(
                select_start.line, select_start.column);
            std::size_t select_end_idx
                = content.index_from_pos(select_end.line, select_end.column);

            if (select_end_idx < render_line_start
                || select_start_idx > render_line_start + render_line_len - 1) {
                continue;
            }

            std::size_t select_start_col
                = std::max(render_line_start, select_start_idx)
                - render_line_start;
            std::size_t select_end_col
                = std::min(render_line_start + render_line_len - 1,
                           select_end_idx)
                - render_line_start;

            float select_x
                = constants::margin
                + (header_width + 1 + select_start_col - view.offset_column())
                      * char_size.x;
            int select_len = select_end_col - select_start_col + 1;

            if (select_len == 0) {
                select_len = 1;
            }

            float select_width = select_len * char_size.x;

            DrawRectangle(select_x, y, select_width, line_height,
                          ColorAlpha(GRAY, 0.3F));
        }
    }

    // draw cursorline
    const float cursor_y
        = constants::margin + (cursor.line - view.offset_line()) * line_height;

    // draw block cursor
    const float cursor_x
        = constants::margin
        + (header_width + 1 + cursor.column - view.offset_column())
              * char_size.x;

    if (view.viewable_line(cursor.line, char_size)) {
        if (m_mode != EditorMode::Visual) {
            // draw cursorline
            DrawRectangle(0, cursor_y, GetScreenWidth(), line_height,
                          ColorAlpha(GRAY, 0.15F));
        }

        // draw block cursor
        if (view.viewable_column(cursor.column, char_size)) {
            DrawRectangle(cursor_x, cursor_y, line_width, line_height,
                          ColorAlpha(ORANGE, 0.45F));
        }
    }

    if (!m_finder.is_active() && m_finder.to_highlight()) {
        m_finder.find_in_content(current_buffer().rope());
        for (const auto& matched_cursor : m_finder.matches()) {
            if (view.viewable(matched_cursor.line, matched_cursor.column,
                              char_size)) {
                // draw cursorline
                const float matched_y
                    = constants::margin
                    + (matched_cursor.line - view.offset_line()) * line_height;

                // draw block cursor
                const float matched_x
                    = constants::margin
                    + (header_width + 1 + matched_cursor.column
                       - view.offset_column())
                          * char_size.x;
                DrawRectangle(matched_x, matched_y,
                              m_finder.pattern().length() * line_width,
                              line_height, ColorAlpha(RED, 0.2));
            }
        }
    }

    current_buffer().suggester().render({cursor_x, cursor_y + line_height});
    m_finder.render();
}

void Editor::render() {
    render_status_bar();
    render_buffer();
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
        return {KEY_LEFT_CONTROL, std::tolower(key)};

    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:
        return {KEY_LEFT_ALT, std::tolower(key)};

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
    case EditorMode::Visual:
        visual_mode(rv);
        break;
    case EditorMode::BufferList:
        buffer_list_mode(rv);
        break;
    case EditorMode::Finder:
        finder_mode(rv);
        break;
    default:
        utils::unreachable();
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

void Editor::set_mode(EditorMode mode) {
    if (mode == EditorMode::Insert) {
        current_buffer().save_snapshot();
    }

    if (mode == EditorMode::BufferList && m_mode != mode) {
        Buffer buffer_list;
        auto& rope = buffer_list.rope();

        rope = Rope{};
        buffer_list.cursor().line = m_buffer_id;

        for (const auto& buffer : m_buffers) {
            if (buffer.dirty()) {
                rope = rope.append("[*] ");
            }
            rope = rope.append(buffer.filename()).append("\n");
        }

        m_buffers.emplace_back(buffer_list);
        m_prev_buffer_id = m_buffer_id;
        m_buffer_id = m_buffers.size() - 1;
    }

    m_mode = mode;
}

void Editor::reset_to_normal_mode() {
    const auto& cursor = current_buffer().cursor();
    const auto& rope = current_buffer().rope();

    if (cursor.column > 0) {
        std::size_t index = rope.index_from_pos(cursor.line, cursor.column);
        if (rope[index] == '\n' || rope[index] == '\0') {
            current_buffer().cursor_move_column(-1, false);
        }
    }

    if (m_mode == EditorMode::BufferList) {
        m_buffer_id = m_prev_buffer_id;
        m_buffers.pop_back();
    }

    set_mode(EditorMode::Normal);
}

void Editor::normal_mode(Key key) {
    if (key.key == KEY_ESCAPE) {
        m_keybinds.reset_step();
        m_finder.set_to_highlight(false);
        return;
    }

    if (m_finder.is_active()) {
        return;
    }

    if (key.modifier != KEY_NULL) {
        m_keybinds.reset_step();
    } else {
        m_keybinds.step(key.key, m_mode != EditorMode::BufferList);
    }
}

void Editor::insert_mode(Key key) {
    if (key.key == KEY_ESCAPE) {
        reset_to_normal_mode();
        current_buffer().suggester().to_render(false);
        return;
    }

    if (key.key == KEY_BACKSPACE) {
        current_buffer().erase_at_cursor();
        current_buffer().suggester().to_render(false);
        return;
    }

    if (key.key == KEY_TAB) {
        current_buffer().insert_at_cursor(std::string(4, ' '));
        current_buffer().suggester().to_render(false);
        return;
    }

    if (key.modifier == KEY_LEFT_CONTROL) {
        auto& buffer = current_buffer();
        auto& cursor = buffer.cursor();
        auto old_cursor = cursor;
        const auto& rope = buffer.rope();

        std::size_t end = rope.index_from_pos(cursor.line, cursor.column);
        buffer.cursor_move_prev_word();
        std::size_t start = rope.index_from_pos(cursor.line, cursor.column);
        cursor = old_cursor;

        std::string cur_word = rope.substr(start, end - start);

        auto& suggester = buffer.suggester();

        if (suggester.should_update()) {
            auto opt_undo_top = buffer.undo_top();
            if (opt_undo_top) {
                suggester.load_text(*opt_undo_top);
            } else {
                suggester.load_text("");
            }
        }

        suggester.set_pattern(cur_word);

        switch (key.key) {
        case 'n':
            suggester.move_next();
            return;
        case 'p':
            suggester.move_prev();
            return;
        }
    }

    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    const auto& view = current_buffer().view();

    if (key.key == '\n') {
        if (current_buffer().suggester().rendering()) {
            auto& buffer = current_buffer();
            auto& cursor = buffer.cursor();
            auto& rope = buffer.rope();

            std::size_t end = rope.index_from_pos(cursor.line, cursor.column);
            buffer.cursor_move_prev_word();
            std::size_t start = rope.index_from_pos(cursor.line, cursor.column);

            auto& suggester = buffer.suggester();

            rope = rope.erase(start, end - start);
            buffer.insert_at_cursor(suggester.select());

            suggester.to_render(false);
        } else {
            current_buffer().insert_at_cursor(std::string{char(key.key)});

            if (current_buffer().cursor().line
                >= view.offset_line() + view.lines(char_size)) {
                current_buffer().cursor_move_line(1);
            } else if (current_buffer().cursor().line < view.offset_line()) {
                current_buffer().cursor_move_line(-1);
            }
            current_buffer().cursor_move_column(-constants::max_line_length,
                                                false);
        }

        return;
    }

    current_buffer().insert_at_cursor(std::string{char(key.key)});
    current_buffer().suggester().mark_update();
    current_buffer().suggester().to_render(false);
}

void Editor::visual_mode(Key key) {
    if (key.key == KEY_ESCAPE) {
        reset_to_normal_mode();
        return;
    }

    if (key.modifier == KEY_NULL) {
        switch (key.key) {
        case 'd':
            current_buffer().erase_selected();
            reset_to_normal_mode();
            return;
        case 'y':
            current_buffer().copy_selected();
            reset_to_normal_mode();
            return;
        default:
            break;
        }
    }

    normal_mode(key);
}

void Editor::buffer_list_mode(Key key) {
    if (key.key == KEY_ESCAPE) {
        m_buffer_id = m_prev_buffer_id;
        reset_to_normal_mode();
        return;
    }

    if (key.modifier == KEY_NULL) {
        m_prev_buffer_id = current_buffer().cursor().line;

        switch (key.key) {
        case '\n':
            reset_to_normal_mode();
            std::cerr << "Switched to buffer " << m_buffer_id << std::endl;
            return;
        case 'd':
            if (m_buffers[m_prev_buffer_id].dirty()) {
                std::cerr << "Buffer " << m_prev_buffer_id
                          << " is modified. Cannot delete." << std::endl;
                return;
            }
            [[fallthrough]];
        case 'D':
            m_buffers.erase(m_buffers.begin() + m_prev_buffer_id);
            std::cerr << "Deleted buffer " << m_prev_buffer_id << std::endl;

            if (m_buffers.size() == 1) {
                std::cerr << "No more buffers. Creating new buffer."
                          << std::endl;
                m_buffers.insert(m_buffers.begin(), Buffer{});
            }

            m_prev_buffer_id
                = std::clamp(0UL, m_prev_buffer_id - 1, m_buffers.size() - 1);
            std::cerr << "Switched to buffer " << m_prev_buffer_id << std::endl;

            reset_to_normal_mode();
            set_mode(EditorMode::BufferList);
            return;
        case 'n':
            // create new buffer
            m_buffers.insert(m_buffers.begin() + m_prev_buffer_id + 1,
                             Buffer{});
            m_prev_buffer_id++;
            std::cerr << "Created new buffer " << m_prev_buffer_id << std::endl;
            reset_to_normal_mode();
            set_mode(EditorMode::BufferList);
            return;
        case 'f':
            reset_to_normal_mode();
            return;
        default:
            break;
        }
    }

    normal_mode(key);
}

void Editor::finder_mode(Key key) {
    if (key.key == KEY_ESCAPE) {
        m_finder.toggle_prompt(FinderMode::None);
        reset_to_normal_mode();
        return;
    }

    if (key.key == KEY_BACKSPACE) {
        m_finder.delete_char();
        return;
    }

    if (key.modifier == KEY_NULL) {
        switch (key.key) {
        case '\n':
            if (m_finder.mode() == FinderMode::Find) {
                m_finder.set_to_highlight(true);
                m_finder.find_in_content(current_buffer().rope());
            } else {
                current_buffer().save_snapshot();
                current_buffer().mark_dirty();
                current_buffer().rope()
                    = m_finder.replace_in_content(current_buffer().rope());
            }

            m_finder.toggle_prompt(FinderMode::None);
            current_buffer().cursor()
                = m_finder.next_match(current_buffer().cursor());
            reset_to_normal_mode();
            return;
        case KEY_TAB:
            if (m_finder.mode() == FinderMode::Replace) {
                m_finder.switch_buffer();
            }
            return;
        default:
            m_finder.append_char(key.key);
        }
    }
}

void Editor::undo() { current_buffer().undo(); }

void Editor::redo() { current_buffer().redo(); }

void Editor::open_file_dialog() {
    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;

    nfdresult_t result
        = NFD::OpenDialog(outPath, nullptr, 0, GetWorkingDirectory());
    if (result == NFD_OKAY) {
        if (m_mode == EditorMode::BufferList) {
            m_buffers.pop_back();
        }
        open(outPath.get());
        std::cout << "Opened " << outPath.get() << std::endl;
        set_mode(EditorMode::Normal);
    } else if (result == NFD_CANCEL) {
        std::cout << "User pressed cancel." << std::endl;
    } else {
        std::cout << "Error: " << NFD::GetError() << std::endl;
    }
}
