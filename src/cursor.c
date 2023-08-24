#include "cursor.h"

int cursor_cmp(Cursor a, Cursor b) {
    if (a.line != b.line) {
        return a.line - b.line;
    }
    return a.column - b.column;
}