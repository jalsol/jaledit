#include "editor.h"

#include "buffer.h"
#include "common/constants.h"
#include "common/vector.h"
#include "finder/finder.h"
#include "highlight/lexer.h"
#include "keybind/trie.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct Editor {
    Vec_BufferPtr *buffers;
    size_t buffer_id;
    size_t prev_buffer_id;
    EditorMode mode;
    Keybind *keybinds;
    Finder *finder;
};

void editor_move_cursor_left(Editor *editor);
void editor_move_cursor_down(Editor *editor);
void editor_move_cursor_up(Editor *editor);
void editor_move_cursor_right(Editor *editor);
void editor_move_cursor_to_first_line(Editor *editor);
void editor_move_cursor_to_last_line(Editor *editor);
void editor_move_cursor_to_first_column(Editor *editor);
void editor_move_cursor_to_last_column(Editor *editor);
void editor_move_cursor_to_next_word(Editor *editor);
void editor_move_cursor_to_prev_word(Editor *editor);
void editor_insert_mode(Editor *editor);
void editor_visual_mode(Editor *editor);

void editor_populate_keybinds(Keybind *keybind) {
    keybind_add(keybind, "h", 1, false, editor_move_cursor_left);
    keybind_add(keybind, "j", 1, false, editor_move_cursor_down);
    keybind_add(keybind, "k", 1, false, editor_move_cursor_up);
    keybind_add(keybind, "l", 1, false, editor_move_cursor_right);
    keybind_add(keybind, "0", 1, false, editor_move_cursor_to_first_column);
    keybind_add(keybind, "$", 1, false, editor_move_cursor_to_last_column);
    keybind_add(keybind, "w", 1, false, editor_move_cursor_to_next_word);
    keybind_add(keybind, "b", 1, false, editor_move_cursor_to_prev_word);
    keybind_add(keybind, "i", 1, false, editor_insert_mode);
    keybind_add(keybind, "v", 1, false, editor_visual_mode);
    keybind_add(keybind, "gg", 2, false, editor_move_cursor_to_first_line);
    keybind_add(keybind, "G", 1, false, editor_move_cursor_to_last_line);
}

Editor *editor_new() {
    Editor *editor = malloc(sizeof(Editor));
    editor->buffers = vec_BufferPtr_new();
    editor->buffer_id = 0;
    editor->prev_buffer_id = 0;
    editor->mode = EDITOR_NORMAL;
    editor->keybinds = keybind_new();
    editor_populate_keybinds(editor->keybinds);
    editor->finder = finder_new();
    return editor;
}

Editor *editor_new_from_file(const char *filename) {
    Editor *editor = editor_new();
    editor_open(editor, filename);
    return editor;
}

void editor_delete(Editor *editor) {
    for (size_t i = 0; i < vec_BufferPtr_size(editor->buffers); i++) {
        buffer_delete(vec_BufferPtr_at(editor->buffers, i));
    }
    vec_BufferPtr_delete(editor->buffers);
    keybind_delete(editor->keybinds);
    finder_delete(editor->finder);
    free(editor);
}

Buffer *editor_cur_buffer(Editor *editor) {
    return vec_BufferPtr_at(editor->buffers, editor->buffer_id);
}

void editor_render_status_bar(Editor *editor) {
    static const char *modes[] = {
        "NORMAL", "INSERT", "VISUAL", "BUFFER LIST", "FINDER",
    };

    const char *status = modes[editor->mode];

    draw_text(status, (Vector2){MARGIN, 0}, BLACK, FONT_SIZE, 0);

    // draw file name
    if (editor->mode != EDITOR_BUFFER_LIST) {
        const char *filename = buffer_filename(editor_cur_buffer(editor));
        float filename_width = measure_text(filename, FONT_SIZE, 0).x;

        draw_text(filename, (Vector2){GetScreenWidth() - MARGIN - filename_width, 0},
                  BLACK, FONT_SIZE, 0);

        if (buffer_dirty(editor_cur_buffer(editor))) {
            draw_text("*", (Vector2){(float)GetScreenWidth() - MARGIN, 0}, BLACK,
                      FONT_SIZE, 0);
        }
    }

    // draw status background
    DrawRectangle(0, 0, GetScreenWidth(), MARGIN, ColorAlpha(ORANGE, 0.2F));
}

void editor_render_buffer(Editor *editor) {
    const Vector2 char_size = measure_text(" ", FONT_SIZE, 0);
    const int line_height = FONT_SIZE + LINE_SPACING;
    const size_t line_width = char_size.x;

    Cursor cursor = buffer_cursor(editor_cur_buffer(editor));
    Rope *content = buffer_rope(editor_cur_buffer(editor));
    View view = buffer_view(editor_cur_buffer(editor));

    const int max_line_number_size = num_len(rope_line_cnt(content));
    const int offset_from_number = 2;
    const int header_width = max_line_number_size + offset_from_number;

    view.header_size = header_width;

    if (!view_is_line_viewable(view, cursor.line, char_size)) {
        view.offset_line = cursor.line;

        if (!view_is_column_viewable(view, cursor.column, char_size)) {
            view.offset_column = cursor.column;
        }
    }
    buffer_set_view(editor_cur_buffer(editor), view);

    // draw text and line numbers
    float y = MARGIN - line_height;
    size_t cur_line_idx = view.offset_line;
    size_t line_start = rope_find_line_start(content, cur_line_idx);
    size_t next_line_start;

    for (; cur_line_idx < rope_line_cnt(content);
         ++cur_line_idx, line_start = next_line_start) {
        next_line_start = rope_find_line_start(content, cur_line_idx + 1);

        if (!view_is_line_viewable(view, cur_line_idx, char_size)) {
            break;
        }

        y += line_height;

        size_t render_line_start = line_start + view.offset_column;
        size_t line_len = next_line_start - line_start;

        if (cur_line_idx + 1 == rope_line_cnt(content)) {
            ++line_len;
        }

        size_t render_line_len =
            MIN(line_len - 1, (size_t)viewable_columns_cnt(view, char_size) - 1);

        // printf("line %zu, line_start: %zu, line_len: %zu, render_line_start: %zu, "
        //        "render_line_len: %zu\n",
        //        cur_line_idx, line_start, line_len, render_line_start, render_line_len);

        Vec_char *line = rope_substr(content, render_line_start, render_line_len);
        const char *line_number = TextFormat("%-*d", header_width + 1, cur_line_idx + 1);

        draw_text(line_number, (Vector2){MARGIN, y}, BLACK, FONT_SIZE, 0);

        float x = MARGIN + (header_width + 1) * char_size.x;
        Lexer *lexer = lexer_new(vec_char_data(line));
        Token token;

        while (true) {
            token = lexer_next(lexer);

            if (token.kind == TOKEN_END) {
                break;
            }

            const char *text = (strncmp(token.start, "\t", 1) == 0)
                                   ? "        "
                                   : TextFormat("%.*s", token.length, token.start);

            draw_text(text, (Vector2){x, y}, token_kind_color(token.kind), FONT_SIZE, 0);

            x += strlen(text) * char_size.x;
        }

        lexer_delete(lexer);
        vec_char_delete(line);

        if (editor->mode == EDITOR_VISUAL) {
            Cursor sel_start = buffer_select_start(editor_cur_buffer(editor));
            Cursor sel_end = buffer_select_end(editor_cur_buffer(editor));

            size_t select_start_idx =
                rope_index_from_pos(content, sel_start.line, sel_start.column);
            size_t select_end_idx =
                rope_index_from_pos(content, sel_end.line, sel_end.column);

            if (select_end_idx < render_line_start ||
                select_start_idx > render_line_start + render_line_len - 1) {
                continue;
            }

            size_t select_start_col =
                MAX(render_line_start, select_start_idx) - render_line_start;
            size_t select_end_col =
                MIN(render_line_start + render_line_len - 1, select_end_idx) -
                render_line_start;

            float select_x =
                MARGIN +
                (header_width + 1 + select_start_col - view.offset_column) * char_size.x;
            int select_len = select_end_col - select_start_col + 1;

            if (select_len == 0) {
                select_len = 1;
            }

            float select_width = select_len * char_size.x;

            DrawRectangle(select_x, y, select_width, line_height, ColorAlpha(GRAY, 0.3F));
        }
    }

    // draw cursorline
    const float cursor_y = MARGIN + (cursor.line - view.offset_line) * line_height;

    // draw block cursor
    const float cursor_x =
        MARGIN + (header_width + 1 + cursor.column - view.offset_column) * char_size.x;

    if (view_is_line_viewable(view, cursor.line, char_size)) {
        if (editor->mode != EDITOR_VISUAL) {
            // draw cursorline
            DrawRectangle(0, cursor_y, GetScreenWidth(), line_height,
                          ColorAlpha(GRAY, 0.15F));
        }

        // draw block cursor
        if (view_is_column_viewable(view, cursor.column, char_size)) {
            DrawRectangle(cursor_x, cursor_y, line_width, line_height,
                          ColorAlpha(ORANGE, 0.45F));
        }
    }

    if (!finder_is_active(editor->finder) && finder_should_highlight(editor->finder)) {
        finder_find_in_content(editor->finder, buffer_rope(editor_cur_buffer(editor)));
        Vec_Cursor *matches = finder_matches(editor->finder);

        for (size_t i = 0; i < vec_Cursor_size(matches); i++) {
            Cursor matched_cursor = vec_Cursor_at(matches, i);

            if (view_is_line_viewable(view, matched_cursor.line, char_size)) {
                // draw cursorline
                const float matched_y =
                    MARGIN + (matched_cursor.line - view.offset_line) * line_height;

                // draw block cursor
                const float matched_x =
                    MARGIN +
                    (header_width + 1 + matched_cursor.column - view.offset_column) *
                        char_size.x;
                DrawRectangle(matched_x, matched_y,
                              vec_char_size(finder_pattern(editor->finder)) * line_width,
                              line_height, ColorAlpha(RED, 0.2));
            }
        }

        vec_Cursor_delete(matches);
    }

    // suggester_render(buffer_suggester(editor_cur_buffer(editor)),
    //                  (Vector2){cursor_x, cursor_y + line_height});
    // finder_render(editor->finder);
}

void editor_render(Editor *editor) {
    editor_render_status_bar(editor);
    editor_render_buffer(editor);
}

typedef struct ModMap {
    int from;
    int to;
} ModMap;

static const ModMap map[] = {
    {',', '<'},  {'.', '>'}, {'/', '?'}, {';', ':'}, {'\'', '"'}, {'[', '{'}, {']', '}'},
    {'\\', '|'}, {'`', '~'}, {'-', '_'}, {'=', '+'}, {'1', '!'},  {'2', '@'}, {'3', '#'},
    {'4', '$'},  {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},  {'9', '('}, {'0', ')'},
};

#define MOD_MAP_SIZE (sizeof(map) / sizeof(map[0]))

int shift(int key) {
    if (isalpha(key)) {
        return toupper(key);
    }

    for (size_t i = 0; i < MOD_MAP_SIZE; i++) {
        if (map[i].from == key) {
            return map[i].to;
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
        return (Key){KEY_NULL, shift(key)};

    case KEY_LEFT_CONTROL:
    case KEY_RIGHT_CONTROL:
        return (Key){KEY_LEFT_CONTROL, tolower(key)};

    case KEY_LEFT_ALT:
    case KEY_RIGHT_ALT:
        return (Key){KEY_LEFT_ALT, tolower(key)};

    default:
        return (Key){KEY_NULL, tolower(key)};
    }
}

void editor_open(Editor *editor, const char *filename) {
    if (strcmp(filename, "") == 0) {
        vec_BufferPtr_push_back(editor->buffers, buffer_new());
    } else {
        vec_BufferPtr_push_back(editor->buffers, buffer_new_from_file(filename));
    }

    editor->buffer_id = vec_BufferPtr_size(editor->buffers) - 1;
}

void editor_set_mode(Editor *editor, EditorMode mode) {
    if (mode == EDITOR_INSERT) {
        buffer_save_snapshot(editor_cur_buffer(editor));
    }

    if (mode == EDITOR_BUFFER_LIST && editor->mode != mode) {
        Buffer *buffer_list = buffer_new();

        // clear buffer list
        {
            Rope *rope = rope_new();
            rope_delete(buffer_rope(buffer_list));
            buffer_set_rope(buffer_list, rope);
        }

        // set cursor to current buffer
        {
            Cursor cursor = buffer_cursor(buffer_list);
            cursor.line = editor->buffer_id;
            buffer_set_cursor(buffer_list, cursor);
        }

        for (size_t i = 0; i < vec_BufferPtr_size(editor->buffers); i++) {
            Buffer *buffer = vec_BufferPtr_at(editor->buffers, i);

            if (buffer_dirty(buffer)) {
                Rope *tmp = rope_append_text(buffer_rope(buffer), "[*]", 3);
                rope_delete(buffer_rope(buffer));
                buffer_set_rope(buffer, tmp);
            }

            Rope *tmp1 = rope_append_text(buffer_rope(buffer), buffer_filename(buffer),
                                          strlen(buffer_filename(buffer)));
            Rope *tmp2 = rope_append_text(tmp1, "\n", 1);
            rope_delete(tmp1);
            rope_delete(buffer_rope(buffer));
            buffer_set_rope(buffer, tmp2);
        }

        vec_BufferPtr_push_back(editor->buffers, buffer_list);
        editor->prev_buffer_id = editor->buffer_id;
        editor->buffer_id = vec_BufferPtr_size(editor->buffers) - 1;
    }

    editor->mode = mode;
}

void editor_reset_to_normal(Editor *editor) {
    Cursor cursor = buffer_cursor(editor_cur_buffer(editor));
    Rope *rope = buffer_rope(editor_cur_buffer(editor));

    if (cursor.column > 0) {
        size_t index = rope_index_from_pos(rope, cursor.line, cursor.column);
        if (rope_at(rope, index) == '\n' || rope_at(rope, index) == '\0') {
            buffer_cursor_move_column(editor_cur_buffer(editor), -1, false);
        }
    }

    if (editor->mode == EDITOR_BUFFER_LIST) {
        editor->buffer_id = editor->prev_buffer_id;
        buffer_delete(vec_BufferPtr_back(editor->buffers));
        vec_BufferPtr_pop_back(editor->buffers);
    }

    editor_set_mode(editor, EDITOR_NORMAL);
}

void editor_process_normal(Editor *editor, Key key) {
    if (key.key == KEY_ESCAPE) {
        keybind_reset(editor->keybinds);
        finder_set_to_highlight(editor->finder, false);
        return;
    }

    if (finder_is_active(editor->finder)) {
        return;
    }

    if (key.modifier != KEY_NULL) {
        keybind_reset(editor->keybinds);
    } else {
        keybind_step(editor->keybinds, editor, key.key,
                     editor->mode != EDITOR_BUFFER_LIST);
    }
}

void editor_process_insert(Editor *editor, Key key) {
    if (key.key == KEY_ESCAPE) {
        editor_reset_to_normal(editor);
        suggester_set_to_render(buffer_suggester(editor_cur_buffer(editor)), false);
        return;
    }

    if (key.key == KEY_BACKSPACE) {
        buffer_erase_at_cursor(editor_cur_buffer(editor));
        suggester_set_to_render(buffer_suggester(editor_cur_buffer(editor)), false);
        return;
    }

    if (key.key == KEY_TAB) {
        Rope *tmp = rope_append_text(buffer_rope(editor_cur_buffer(editor)), "    ", 4);
        buffer_set_rope(editor_cur_buffer(editor), tmp);
        suggester_set_to_render(buffer_suggester(editor_cur_buffer(editor)), false);
        return;
    }

    if (key.modifier == KEY_LEFT_CONTROL) {
        Buffer *buffer = editor_cur_buffer(editor);
        Cursor cursor = buffer_cursor(buffer);
        Cursor old_cursor = cursor;
        Rope *rope = buffer_rope(buffer);

        size_t end = rope_index_from_pos(rope, cursor.line, cursor.column);
        buffer_cursor_move_prev_word(buffer);
        size_t start = rope_index_from_pos(rope, cursor.line, cursor.column);
        buffer_set_cursor(buffer, old_cursor);

        Vec_char *cur_word = rope_substr(rope, start, end - start);
        Suggester *suggester = buffer_suggester(buffer);

        if (suggester_should_update(suggester)) {
            suggester_load_text(suggester, buffer_undo_top(buffer));
        }

        suggester_set_pattern(suggester, vec_char_data(cur_word),
                              vec_char_size(cur_word));

        vec_char_delete(cur_word);

        switch (key.key) {
        case 'n':
            suggester_select_next(suggester);
            return;
        case 'p':
            suggester_select_prev(suggester);
            return;
        }
    }

    const Vector2 char_size = measure_text(" ", FONT_SIZE, 0);
    View view = buffer_view(editor_cur_buffer(editor));

    if (key.key == '\n') {
        if (suggester_is_rendering(buffer_suggester(editor_cur_buffer(editor)))) {
            Buffer *buffer = editor_cur_buffer(editor);
            Cursor cursor = buffer_cursor(buffer);
            Rope *rope = buffer_rope(buffer);

            size_t end = rope_index_from_pos(rope, cursor.line, cursor.column);
            buffer_cursor_move_prev_word(buffer);
            size_t start = rope_index_from_pos(rope, cursor.line, cursor.column);

            Suggester *suggester = buffer_suggester(buffer);

            Rope *tmp1 = rope_erase(rope, start, end - start);
            Rope *tmp2 = rope_insert_text(tmp1, start, suggester_get_selected(suggester),
                                          strlen(suggester_get_selected(suggester)));
            rope_delete(tmp1);
            rope_delete(buffer_rope(buffer));
            buffer_set_rope(buffer, tmp2);

            suggester_set_to_render(suggester, false);
        } else {
            Rope *tmp = rope_append_text(buffer_rope(editor_cur_buffer(editor)), "\n", 1);
            buffer_set_rope(editor_cur_buffer(editor), tmp);

            Cursor cursor = buffer_cursor(editor_cur_buffer(editor));

            if (cursor.line >= view.offset_line + viewable_lines_cnt(char_size)) {
                buffer_cursor_move_line(editor_cur_buffer(editor), 1);
            } else if (cursor.line < view.offset_line) {
                buffer_cursor_move_line(editor_cur_buffer(editor), -1);
            }

            buffer_cursor_move_column(editor_cur_buffer(editor), -MAX_LINE_LENGTH, false);
        }

        return;
    }

    Rope *tmp =
        rope_append_text(buffer_rope(editor_cur_buffer(editor)), (char *)&key.key, 1);
    buffer_set_rope(editor_cur_buffer(editor), tmp);

    Suggester *suggester = buffer_suggester(editor_cur_buffer(editor));
    suggester_should_update(suggester);
    suggester_set_to_render(suggester, false);
}

void editor_process_visual(Editor *editor, Key key) {
    if (key.key == KEY_ESCAPE) {
        editor_reset_to_normal(editor);
        return;
    }

    if (key.modifier == KEY_NULL) {
        switch (key.key) {
        case 'd':
            buffer_erase_selection(editor_cur_buffer(editor));
            editor_reset_to_normal(editor);
            return;
        case 'y':
            buffer_copy_selection(editor_cur_buffer(editor));
            editor_reset_to_normal(editor);
            return;
        default:
            break;
        }
    }

    editor_process_normal(editor, key);
}

void editor_process_buffer_list(Editor *editor, Key key) {
    if (key.key == KEY_ESCAPE) {
        editor->buffer_id = editor->prev_buffer_id;
        editor_reset_to_normal(editor);
        return;
    }

    if (key.modifier == KEY_NULL) {
        editor->prev_buffer_id = buffer_cursor(editor_cur_buffer(editor)).line;

        switch (key.key) {
        case '\n':
            editor_reset_to_normal(editor);
            return;
        case 'd':
            if (buffer_dirty(vec_BufferPtr_at(editor->buffers, editor->prev_buffer_id))) {
                return;
            }
            __attribute__((fallthrough));
        case 'D':
            buffer_delete(vec_BufferPtr_at(editor->buffers, editor->prev_buffer_id));
            vec_BufferPtr_erase(editor->buffers, editor->prev_buffer_id);

            if (vec_BufferPtr_size(editor->buffers) == 1) {
                vec_BufferPtr_insert(editor->buffers, 0, buffer_new());
            }

            editor->prev_buffer_id = CLAMP(0, editor->prev_buffer_id - 1,
                                           vec_BufferPtr_size(editor->buffers) - 1);
            editor_reset_to_normal(editor);
            editor_set_mode(editor, EDITOR_BUFFER_LIST);
            return;
        case 'n':
            vec_BufferPtr_insert(editor->buffers, editor->prev_buffer_id + 1,
                                 buffer_new());
            editor->prev_buffer_id++;
            editor_reset_to_normal(editor);
            editor_set_mode(editor, EDITOR_BUFFER_LIST);
            return;
        case 'f':
            editor_reset_to_normal(editor);
            return;
        default:
            break;
        }
    }

    editor_process_normal(editor, key);
}

void editor_process_finder(Editor *editor, Key key) {
    if (key.key == KEY_ESCAPE) {
        finder_toggle_prompt(editor->finder, FINDER_NONE);
        editor_reset_to_normal(editor);
        return;
    }

    if (key.key == KEY_BACKSPACE) {
        finder_pop_char(editor->finder);
        return;
    }

    if (key.modifier == KEY_NULL) {
        switch (key.key) {
        case '\n':
            if (finder_mode(editor->finder) == FINDER_FIND) {
                finder_set_to_highlight(editor->finder, true);
                finder_find_in_content(editor->finder,
                                       buffer_rope(editor_cur_buffer(editor)));
            } else {
                buffer_save_snapshot(editor_cur_buffer(editor));
                buffer_set_dirty(editor_cur_buffer(editor), true);

                Rope *tmp = finder_replace_in_content(
                    editor->finder, buffer_rope(editor_cur_buffer(editor)));
                buffer_set_rope(editor_cur_buffer(editor), tmp);
            }

            finder_toggle_prompt(editor->finder, FINDER_NONE);
            buffer_set_cursor(
                editor_cur_buffer(editor),
                finder_next_match(editor->finder,
                                  buffer_cursor(editor_cur_buffer(editor))));
            editor_reset_to_normal(editor);
            return;
        case KEY_TAB:
            if (finder_mode(editor->finder) == FINDER_REPLACE) {
                finder_switch_active_input(editor->finder);
            }
            return;
        default:
            finder_append_char(editor->finder, key.key);
        }
    }
}

void editor_update(Editor *editor) {
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
        rv = (Key){KEY_NULL, key};
    }

    switch (editor->mode) {
    case EDITOR_NORMAL:
        editor_process_normal(editor, rv);
        break;
    case EDITOR_INSERT:
        editor_process_insert(editor, rv);
        break;
    case EDITOR_VISUAL:
        editor_process_visual(editor, rv);
        break;
    case EDITOR_BUFFER_LIST:
        editor_process_buffer_list(editor, rv);
        break;
    case EDITOR_FINDER:
        editor_process_finder(editor, rv);
        break;
    }
}

void editor_open_file_dialog(Editor *editor) {
    (void)editor;
    assert(0 && "Not implemented");
}

// keybinds

void editor_move_cursor_left(Editor *editor) {
    buffer_cursor_move_column(editor_cur_buffer(editor), -1,
                              editor->mode == EDITOR_VISUAL);
}

void editor_move_cursor_down(Editor *editor) {
    buffer_cursor_move_line(editor_cur_buffer(editor), 1);
}

void editor_move_cursor_up(Editor *editor) {
    buffer_cursor_move_line(editor_cur_buffer(editor), -1);
}

void editor_move_cursor_right(Editor *editor) {
    buffer_cursor_move_column(editor_cur_buffer(editor), 1,
                              editor->mode == EDITOR_VISUAL);
}

void editor_move_cursor_to_first_line(Editor *editor) {
    buffer_cursor_move_line(editor_cur_buffer(editor),
                            -buffer_cursor(editor_cur_buffer(editor)).line);
}

void editor_move_cursor_to_last_line(Editor *editor) {
    buffer_cursor_move_line(editor_cur_buffer(editor),
                            rope_line_cnt(buffer_rope(editor_cur_buffer(editor))) -
                                buffer_cursor(editor_cur_buffer(editor)).line);
}

void editor_move_cursor_to_first_column(Editor *editor) {
    buffer_cursor_move_column(editor_cur_buffer(editor),
                              -buffer_cursor(editor_cur_buffer(editor)).column,
                              editor->mode == EDITOR_VISUAL);
}

void editor_move_cursor_to_last_column(Editor *editor) {
    buffer_cursor_move_column(
        editor_cur_buffer(editor),
        rope_line_len(buffer_rope(editor_cur_buffer(editor)),
                      buffer_cursor(editor_cur_buffer(editor)).line) -
            buffer_cursor(editor_cur_buffer(editor)).column,
        editor->mode == EDITOR_VISUAL);
}

void editor_move_cursor_to_next_word(Editor *editor) {
    buffer_cursor_move_next_word(editor_cur_buffer(editor));
}

void editor_move_cursor_to_prev_word(Editor *editor) {
    buffer_cursor_move_prev_word(editor_cur_buffer(editor));
}

void editor_insert_mode(Editor *editor) { editor_set_mode(editor, EDITOR_INSERT); }

void editor_visual_mode(Editor *editor) {
    buffer_set_select_orig(editor_cur_buffer(editor),
                           buffer_cursor(editor_cur_buffer(editor)));
    editor_set_mode(editor, EDITOR_VISUAL);
}
