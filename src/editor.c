#include "editor.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "keybind.h"
#include "raylib.h"
#include "utils.h"

Editor editor;

Editor editor_construct(const char *file_path) {
    Editor editor;
    editor.mode = MODE_NORMAL;
    editor.buffer = buffer_construct(file_path);
    editor.keybind_trie = keybind_trie_construct();
    return editor;
}

void editor_destruct(Editor *editor) { buffer_destruct(&editor->buffer); }

void editor_render(Editor *editor) {
    Vector2 char_size = measure_text(" ", FONT_SIZE, 0);
    int line_height = FONT_SIZE + 2;
    int line_width = char_size.x;

    int max_line_number_size = number_len(editor->buffer.rows_len);
    int offset_from_number = 2;

    // draw cursorline
    int cursorline_y = MARGIN + editor->buffer.cursor.y * line_height;
    DrawRectangle(0, cursorline_y, GetScreenWidth(), line_height, ColorAlpha(GRAY, 0.2f));

    // draw text and line numbers
    int y = MARGIN;
    for (RowNode *row_node = editor->buffer.row_node_head; row_node != NULL;
         row_node = row_node->next) {
        if (y > GetScreenHeight()) {
            break;
        }

        const char *line_text =
            TextFormat("%-*d%s", max_line_number_size + offset_from_number,
                       row_node->row.index, row_node->row.content);

        draw_text(line_text, (Vector2){MARGIN, y}, BLACK, FONT_SIZE, 0);
        y += line_height;
    }

    // draw block cursor
    int cursor_x =
        MARGIN + (max_line_number_size + offset_from_number + editor->buffer.cursor.x) *
                     char_size.x;
    int cursor_y = MARGIN + editor->buffer.cursor.y * line_height;
    DrawRectangle(cursor_x, cursor_y, line_width, line_height, BLACK);
    draw_text(TextFormat("%c", editor->buffer.cursor.c), (Vector2){cursor_x, cursor_y},
              WHITE, FONT_SIZE, 0);
}

void editor_update(Editor *editor) {
    switch (editor->mode) {
    case MODE_NORMAL:
        editor_update_normal_mode(editor);
        break;
    case MODE_INSERT:
        // editor_update_insert_mode(editor);
        // break;
    default:
        assert(0 && "unreachable");
    }
}

void editor_update_normal_mode(Editor *editor) {
    // bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    char c = GetCharPressed();

    if (c == '\0') {
        return;
    }

    printf("key pressed: %c\n", c);
    editor_keybind_trie_step(editor, c);
}

// void editor_update_insert_mode(Editor *editor) {}

void editor_keybind_trie_step(Editor *editor, char chr) {
    KeybindTrieNode **ptr = &(editor->keybind_trie.ptr);

    if (*ptr == NULL || (*ptr)->children[(int)chr] == NULL) {
        *ptr = editor->keybind_trie.root;
        return;
    }

    *ptr = (*ptr)->children[(int)chr];

    if ((*ptr)->handler_type == KEYBIND_NONE) {
        return;
    }

    switch ((*ptr)->handler_type) {
    case KEYBIND_BUFFER_MOVE_CURSOR: {
        KeybindHandlerArgs *args = &(*ptr)->handler_args;
        buffer_move_cursor(&editor->buffer, args->buffer_move_cursor.dx,
                           args->buffer_move_cursor.dy);
        break;
    }
    case KEYBIND_BUFFER_MOVE_TO_NEXT_WORD: {
        buffer_move_to_next_word(&editor->buffer);
        break;
    }
    case KEYBIND_BUFFER_MOVE_TO_PREV_WORD: {
        buffer_move_to_prev_word(&editor->buffer);
        break;
    }
    default:
        assert(0 && "unreachable");
    }

    *ptr = editor->keybind_trie.root;
}
