#include "editor.h"

#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "raylib.h"
#include "utils.h"

Editor editor_construct(const char *file_path) {
    Editor editor;
    editor.mode = MODE_COMMAND;
    editor.buffer = buffer_construct(file_path);
    return editor;
}

void editor_destruct(Editor *editor) { buffer_destruct(&editor->buffer); }

void editor_render(Editor *editor) {
    // Draw line numbers
    Vector2 char_size = measure_text(" ", FONT_SIZE, 0);
    int line_height = FONT_SIZE + 2;
    int line_width = char_size.x;

    int max_line_number_size = number_len(editor->buffer.rows_len);

    int y = MARGIN;
    for (RowNode *row = editor->buffer.rows_head; row != NULL;
         row = row->next) {
        const char *line_text = TextFormat("%-*d%s", max_line_number_size + 2,
                                           row->row.index, row->row.content);

        draw_text(line_text, (Vector2){MARGIN, y}, BLACK, FONT_SIZE, 0);
        y += line_height;
    }

    // Draw block cursor
    // int cursor_x = MARGIN + editor->buffer.cursor.x * line_width;
    // int cursor_y = MARGIN + editor->buffer.cursor.y * line_height;

    // DrawRectangle(cursor_x, cursor_y, line_width, line_height, BLACK);
    // draw_text("#", (Vector2){cursor_x, cursor_y}, WHITE, FONT_SIZE, 0);
}