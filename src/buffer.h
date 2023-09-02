#pragma once

#include "autocomplete/suggester.h"
#include "cursor.h"
#include "rope/rope.h"
#include "view.h"

#include <stdbool.h>
#include <stddef.h>

struct Buffer;
typedef struct Buffer Buffer;

Buffer *buffer_new();
Buffer *buffer_new_from_file(const char *filename);
void buffer_delete(Buffer *buffer);

Cursor buffer_cursor(Buffer *buffer);
void buffer_set_cursor(Buffer *buffer, Cursor cursor);
Rope *buffer_rope(Buffer *buffer);
void buffer_set_rope(Buffer *buffer, Rope *rope);
View buffer_view(Buffer *buffer);
void buffer_set_view(Buffer *buffer, View view);
Cursor buffer_select_orig(Buffer *buffer);
void buffer_set_select_orig(Buffer *buffer, Cursor cursor);
Cursor buffer_select_start(Buffer *buffer);
Cursor buffer_select_end(Buffer *buffer);
Suggester *buffer_suggester(Buffer *buffer);
const char *buffer_filename(Buffer *buffer);
bool buffer_dirty(Buffer *buffer);
void buffer_set_dirty(Buffer *buffer, bool dirty);

void buffer_cursor_move_line(Buffer *buffer, int offset);
void buffer_cursor_move_column(Buffer *buffer, int offset, bool move_on_eol);
void buffer_cursor_move_next_char(Buffer *buffer);
void buffer_cursor_move_prev_char(Buffer *buffer);
void buffer_cursor_move_next_word(Buffer *buffer);
void buffer_cursor_move_prev_word(Buffer *buffer);

void buffer_insert_at_cursor(Buffer *buffer, Rope *rope);
void buffer_append_at_cursor(Buffer *buffer, Rope *rope);
void buffer_erase_at_cursor(Buffer *buffer);
void buffer_erase_selection(Buffer *buffer);
void buffer_erase_range(Buffer *buffer, size_t start, size_t end);
void buffer_copy_selection(Buffer *buffer);
void buffer_copy_range(Buffer *buffer, size_t start, size_t end);

void buffer_undo(Buffer *buffer);
Rope *buffer_undo_top(Buffer *buffer);
void buffer_redo(Buffer *buffer);
void buffer_save_snapshot(Buffer *buffer);
void buffer_save_file(Buffer *buffer);
void buffer_save_file_as(Buffer *buffer);