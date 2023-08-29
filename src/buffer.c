#include "buffer.h"

#include "autocomplete/suggester.h"
#include "common/constants.h"
#include "common/utils.h"
#include "common/vector.h"
#include "cursor.h"
#include "rope/rope.h"
#include "snapshot.h"
#include "view.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE (1024 * 1024)

struct Buffer {
    Rope *rope;
    Vec_Snapshot *undo;
    Vec_Snapshot *redo;
    Cursor cursor;
    View view;

    Cursor select_orig;
    const char *filename;
    bool dirty;
    Suggester *suggester;
};

Buffer *buffer_new() {
    Buffer *buffer = malloc(sizeof(Buffer));
    buffer->rope = rope_new_from_text("\n", 1);
    buffer->undo = vec_Snapshot_new();
    buffer->redo = vec_Snapshot_new();
    buffer->cursor = (Cursor){0};
    buffer->view = (View){0, 0, num_len(rope_line_cnt(buffer->rope) + 2)};
    buffer->select_orig = (Cursor){-1, -1};
    buffer->filename = "new file";
    buffer->dirty = false;
    buffer->suggester = suggester_new();
    return buffer;
}

Buffer *buffer_new_from_file(const char *filename) {
    Buffer *buffer = buffer_new();
    buffer->filename = filename;

    {
        FILE *fp = fopen(filename, "r");
        assert(fp != NULL && "Can't open file");
        fclose(fp);
    }

    int fd = open(filename, O_RDONLY, S_IRUSR | S_IWUSR);
    struct stat sb;
    assert(fstat(fd, &sb) != -1 && "Could not get file size");

    if (sb.st_size == 0) {
        return buffer;
    } else {
        rope_delete(buffer->rope);
        buffer->rope = rope_new();
    }

    char *file_in_mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    size_t remaining = sb.st_size;
    char *buf_ptr = file_in_mem;
    Rope *tmp;

    while (remaining >= BUF_SIZE) {
        tmp = rope_append_text(buffer->rope, buf_ptr, BUF_SIZE);
        rope_delete(buffer->rope);
        buffer->rope = tmp;

        buf_ptr += BUF_SIZE;
        remaining -= BUF_SIZE;
    }
    tmp = rope_append_text(buffer->rope, buf_ptr, remaining);
    rope_delete(buffer->rope);
    buffer->rope = tmp;
    tmp = NULL;

    munmap(file_in_mem, sb.st_size);
    close(fd);

    buffer->view.header_size = num_len(rope_line_cnt(buffer->rope));

    return buffer;
}

void buffer_delete(Buffer *buffer) {
    rope_delete(buffer->rope);
    vec_Snapshot_delete(buffer->undo);
    vec_Snapshot_delete(buffer->redo);
    suggester_delete(buffer->suggester);
    free(buffer);
}

Cursor buffer_cursor(Buffer *buffer) { return buffer->cursor; }

void buffer_set_cursor(Buffer *buffer, Cursor cursor) { buffer->cursor = cursor; }

Rope *buffer_rope(Buffer *buffer) { return buffer->rope; }

View buffer_view(Buffer *buffer) { return buffer->view; }

void buffer_set_view(Buffer *buffer, View view) { buffer->view = view; }

Cursor buffer_select_orig(Buffer *buffer) { return buffer->select_orig; }

void buffer_set_select_orig(Buffer *buffer, Cursor cursor) {
    buffer->select_orig = cursor;
}

Cursor buffer_select_start(Buffer *buffer) {
    return (cursor_cmp(buffer->cursor, buffer->select_orig) < 0) ? buffer->cursor
                                                                 : buffer->select_orig;
}

Cursor buffer_select_end(Buffer *buffer) {
    return (cursor_cmp(buffer->cursor, buffer->select_orig) > 0) ? buffer->cursor
                                                                 : buffer->select_orig;
}

Suggester *buffer_suggester(Buffer *buffer) { return buffer->suggester; }

const char *buffer_filename(Buffer *buffer) { return buffer->filename; }

bool buffer_dirty(Buffer *buffer) { return buffer->dirty; }

void buffer_set_dirty(Buffer *buffer, bool dirty) { buffer->dirty = dirty; }

void buffer_cursor_move_line(Buffer *buffer, int offset) {
    const Vector2 char_size = measure_text(" ", FONT_SIZE, 0);

    buffer->cursor.line =
        CLAMP(buffer->cursor.line + offset, 0, rope_line_cnt(buffer->rope) - 1);

    buffer_cursor_move_column(buffer, 0, false);

    if (!view_is_line_viewable(buffer->view, buffer->cursor.line, char_size)) {
        if (offset < 0) {
            buffer->view.offset_line = buffer->cursor.line;
        } else if (offset > 0) {
            buffer->view.offset_line =
                buffer->cursor.line - viewable_lines_cnt(char_size) + 1;
        }
    }
}

void buffer_cursor_move_column(Buffer *buffer, int offset, bool move_on_eol) {
    const Vector2 char_size = measure_text(" ", FONT_SIZE, 0);
    int line_length = rope_line_len(buffer->rope, buffer->cursor.line);

    int right_offset =
        !move_on_eol + (buffer->cursor.line + 1 == rope_line_cnt(buffer->rope));

    buffer->cursor.column =
        CLAMP(buffer->cursor.column + offset, 0, line_length - right_offset);

    if (buffer->cursor.column < 0) {
        buffer->cursor.column = 0;
    }

    if (!view_is_column_viewable(buffer->view, buffer->cursor.column, char_size)) {
        if (offset < 0) {
            buffer->view.offset_column = buffer->cursor.column;
        } else if (offset > 0) {
            buffer->view.offset_column =
                buffer->cursor.column - viewable_columns_cnt(buffer->view, char_size) + 1;
        }
    }
}

void buffer_cursor_move_next_char(Buffer *buffer) {
    if (buffer->cursor.column == rope_line_len(buffer->rope, buffer->cursor.line)) {
        ++buffer->cursor.line;
        buffer->cursor.column = 0;
    } else {
        ++buffer->cursor.column;
    }
}

void buffer_cursor_move_prev_char(Buffer *buffer) {
    if (buffer->cursor.column == 0) {
        --buffer->cursor.line;
        buffer->cursor.column = rope_line_len(buffer->rope, buffer->cursor.line);
    } else {
        --buffer->cursor.column;
    }
}

void buffer_cursor_move_next_word(Buffer *buffer) {
    size_t index =
        rope_index_from_pos(buffer->rope, buffer->cursor.line, buffer->cursor.column);
    char c = rope_at(buffer->rope, index);
    bool alnum_word = !!isalnum(c);
    bool punct_word = !!ispunct(c);

    // NOTE: m_rope.length() contains a terminating null character
    // so we need to subtract 2 to get the last character

    int rope_len = rope_length(buffer->rope);

    // if already in word, move to end of word
    if (alnum_word) {
        while (index + 2 < rope_len && isalnum(rope_at(buffer->rope, index))) {
            buffer_cursor_move_next_char(buffer);
            ++index;
        }
    } else if (index + 2 < rope_len && punct_word) {
        buffer_cursor_move_next_char(buffer);
        ++index;
        if (ispunct(rope_at(buffer->rope, index))) {
            return;
        }
    }

    if (isspace(rope_at(buffer->rope, index))) {
        while (index + 2 < rope_len && isspace(rope_at(buffer->rope, index))) {
            buffer_cursor_move_next_char(buffer);
            ++index;
        }
    }

    if (!view_is_cursor_viewable(buffer->view, buffer->cursor,
                                 measure_text(" ", FONT_SIZE, 0))) {
        ++buffer->view.offset_line;
    }
}

void buffer_cursor_move_prev_word(Buffer *buffer) {
    if (buffer->cursor.line == 0 && buffer->cursor.column == 0) {
        return;
    }

    buffer_cursor_move_prev_char(buffer);
    size_t index =
        rope_index_from_pos(buffer->rope, buffer->cursor.line, buffer->cursor.column);

    while (index > 0 && isspace(rope_at(buffer->rope, index))) {
        buffer_cursor_move_prev_char(buffer);
        --index;
    }

    if (ispunct(rope_at(buffer->rope, index))) {
        return;
    }

    // if already in word, move to beginning of word
    while (index > 0 && isalnum(rope_at(buffer->rope, index - 1))) {
        buffer_cursor_move_prev_char(buffer);
        --index;
    }

    if (view_is_cursor_viewable(buffer->view, buffer->cursor,
                                measure_text(" ", FONT_SIZE, 0))) {
        --buffer->view.offset_line;
    }
}

void buffer_insert_at_cursor(Buffer *buffer, Rope *rope) {
    size_t pos =
        rope_index_from_pos(buffer->rope, buffer->cursor.line, buffer->cursor.column);
    Rope *new_rope = rope_insert(buffer->rope, pos, rope);
    rope_delete(buffer->rope);
    buffer->rope = new_rope;
    buffer->dirty = true;

    for (int _ = rope_length(rope); _ > 0; --_) {
        buffer_cursor_move_next_char(buffer);
    }
}

void buffer_append_at_cursor(Buffer *buffer, Rope *rope) {
    size_t pos =
        rope_index_from_pos(buffer->rope, buffer->cursor.line, buffer->cursor.column);
    Rope *new_rope = rope_insert(buffer->rope, pos + 1, rope);
    rope_delete(buffer->rope);
    buffer->rope = new_rope;
    buffer->dirty = true;

    for (int _ = rope_length(rope); _ > 0; --_) {
        buffer_cursor_move_next_char(buffer);
    }
}

void buffer_erase_at_cursor(Buffer *buffer) {
    size_t pos =
        rope_index_from_pos(buffer->rope, buffer->cursor.line, buffer->cursor.column);
    if (pos == 0) {
        return;
    }

    buffer_save_snapshot(buffer);
    buffer_cursor_move_prev_char(buffer);

    buffer->rope = rope_erase(buffer->rope, pos - 1, pos);
    buffer->dirty = true;
}

void buffer_erase_selection(Buffer *buffer) {
    Cursor sel_start = buffer_select_start(buffer);
    Cursor sel_end = buffer_select_end(buffer);

    size_t start_idx =
        rope_index_from_pos(buffer->rope, sel_start.line, sel_start.column);
    size_t end_idx = rope_index_from_pos(buffer->rope, sel_end.line, sel_end.column);

    buffer_erase_range(buffer, start_idx, end_idx);
}

void buffer_erase_range(Buffer *buffer, size_t start, size_t end) {
    buffer_copy_range(buffer, start, end);
    buffer_save_snapshot(buffer);

    buffer->rope = rope_erase(buffer->rope, start, end);
    buffer->dirty = true;

    buffer_set_cursor(buffer, buffer_select_start(buffer));
    if (rope_length(buffer->rope) == 0) {
        Rope *tmp = rope_new_from_text("\n", 1);
        rope_delete(buffer->rope);
        buffer->rope = tmp;
    }
}

void buffer_copy_selection(Buffer *buffer) {
    Cursor sel_start = buffer_select_start(buffer);
    Cursor sel_end = buffer_select_end(buffer);

    size_t start_idx =
        rope_index_from_pos(buffer->rope, sel_start.line, sel_start.column);
    size_t end_idx = rope_index_from_pos(buffer->rope, sel_end.line, sel_end.column);
    buffer_copy_range(buffer, start_idx, end_idx);

    buffer_set_cursor(buffer, buffer_select_start(buffer));
}

void buffer_copy_range(Buffer *buffer, size_t start, size_t end) {
    Vec_char *selection = rope_substr(buffer->rope, start, end);
    SetClipboardText(vec_char_data(selection));
    vec_char_delete(selection);
}

void buffer_save_snapshot(Buffer *buffer) {
    {
        for (size_t i = 0; i < vec_Snapshot_size(buffer->redo); ++i) {
            rope_delete(vec_Snapshot_at(buffer->redo, i).rope);
        }

        vec_Snapshot_clear(buffer->redo);
    }

    vec_Snapshot_push_back(buffer->undo, (Snapshot){
                                             .rope = buffer->rope,
                                             .cursor = buffer->cursor,
                                             .dirty = buffer->dirty,
                                         });
}

void buffer_undo(Buffer *buffer) {
    if (vec_Snapshot_size(buffer->undo) == 0) {
        return;
    }

    vec_Snapshot_push_back(buffer->redo, (Snapshot){
                                             .rope = buffer->rope,
                                             .cursor = buffer->cursor,
                                             .dirty = buffer->dirty,
                                         });

    Snapshot snapshot = vec_Snapshot_back(buffer->undo);
    buffer->rope = snapshot.rope;
    buffer->cursor = snapshot.cursor;
    buffer_set_cursor(buffer, buffer->cursor);

    vec_Snapshot_pop_back(buffer->undo);
}

void buffer_redo(Buffer *buffer) {
    if (vec_Snapshot_size(buffer->redo) == 0) {
        return;
    }

    vec_Snapshot_push_back(buffer->undo, (Snapshot){
                                             .rope = buffer->rope,
                                             .cursor = buffer->cursor,
                                             .dirty = buffer->dirty,
                                         });

    Snapshot snapshot = vec_Snapshot_back(buffer->redo);
    buffer->rope = snapshot.rope;
    buffer->cursor = snapshot.cursor;
    buffer_set_cursor(buffer, buffer->cursor);

    vec_Snapshot_pop_back(buffer->redo);
}

void buffer_save_file(Buffer *buffer) {
    if (strcmp(buffer->filename, "new file") == 0) {
        buffer_save_file_as(buffer);
        return;
    }

    if (!buffer->dirty) {
        return;
    }

    int fd = open(buffer->filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    assert(fd != -1 && "Could not open file");

    size_t text_size = rope_length(buffer->rope);

    if (lseek(fd, text_size - 1, SEEK_SET) == -1) {
        close(fd);
        assert(false && "Could not seek to end of file");
    }

    if (write(fd, "", 1) == -1) {
        close(fd);
        assert(false && "Could not write to file");
    }

    char *map = mmap(0, text_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (map == MAP_FAILED) {
        close(fd);
        assert(false && "Could not map file");
    }

    size_t i;

    for (i = 0; i + BUF_SIZE < text_size; i += BUF_SIZE) {
        Vec_char *buf = rope_substr(buffer->rope, i, BUF_SIZE);
        strncpy(map + i, vec_char_data(buf), BUF_SIZE);
        vec_char_delete(buf);
    }

    Vec_char *buf = rope_substr(buffer->rope, i, text_size - i);
    strncpy(map + i, vec_char_data(buf), text_size - i);

    if (msync(map, text_size, MS_SYNC) == -1) {
        close(fd);
        munmap(map, text_size);
        assert(false && "Could not sync file");
    }

    if (munmap(map, text_size) == -1) {
        close(fd);
        assert(false && "Could not unmap file");
    }

    close(fd);
    buffer->dirty = false;
}

void buffer_save_file_as(Buffer *buffer) {
    (void)buffer;
    assert(false && "Not implemented");
}