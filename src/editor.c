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

void editor_destruct(Editor *editor) {
    buffer_destruct(&editor->buffer);
    keybind_trie_destruct(&editor->keybind_trie);
}

void editor_render(Editor *editor) {
    Vector2 char_size = measure_text(" ", FONT_SIZE, 0);
    const int line_height = FONT_SIZE + 2;
    const int line_width = char_size.x;

    const int max_line_number_size = number_len(editor->buffer.piece_table.total_lines);
    const int offset_from_number = 2;

    // draw cursorline
    const int cursorline_y = MARGIN + editor->buffer.cursor.y * line_height;
    DrawRectangle(0, cursorline_y, GetScreenWidth(), line_height, ColorAlpha(GRAY, 0.2f));

    // draw text and line numbers
    int y = MARGIN - line_height;
    int line_number = 0;
    int line_break_pos = -1;

    for (Piece *piece = editor->buffer.piece_table.pieces_head; piece != NULL;
         piece = piece->next) {
        for (int i = 0; i < (int)piece->length; ++i) {
            char c;
            if (piece->type == PIECE_ADD) {
                c = editor->buffer.piece_table.add[i + piece->start];
            } else {
                c = editor->buffer.piece_table.original[i + piece->start];
            }

            if (c == '\n' || line_number == 0) {
                ++line_number;
                y += line_height;
                draw_text(TextFormat("%-*d", max_line_number_size + offset_from_number,
                                     line_number),
                          (Vector2){MARGIN, y}, BLACK, FONT_SIZE, 0);
            }

            if (c == '\n') {
                line_break_pos = i;
                continue;
            }

            int x = MARGIN +
                    (max_line_number_size + offset_from_number + i - line_break_pos) *
                        char_size.x;
            draw_text(TextFormat("%c", c), (Vector2){x, y}, BLACK, FONT_SIZE, 0);
        }
    }

    // draw block cursor
    const int cursor_x = MARGIN + (max_line_number_size + offset_from_number +
                                   editor->buffer.cursor.x + 1) *
                                      char_size.x;
    const int cursor_y = MARGIN + editor->buffer.cursor.y * line_height;
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
        break;
    default:
        assert(0 && "unreachable");
    }
}

void editor_update_normal_mode(Editor *editor) {
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
    // case KEYBIND_BUFFER_MOVE_TO_NEXT_WORD: {
    //     buffer_move_to_next_word(&editor->buffer);
    //     break;
    // }
    // case KEYBIND_BUFFER_MOVE_TO_PREV_WORD: {
    //     buffer_move_to_prev_word(&editor->buffer);
    //     break;
    // }
    default:
        assert(0 && "unreachable");
    }

    *ptr = editor->keybind_trie.root;
}