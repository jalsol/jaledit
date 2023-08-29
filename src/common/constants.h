#pragma once

enum Window {
    WINDOW_WIDTH = 1024,
    WINDOW_HEIGHT = 768,
};

enum Limit {
    LIMIT_ROW = 50,
    LIMIT_COL = 85,
};

enum {
    FONT_SIZE = 20,
    MARGIN = 20,
    BUFFER_LIMIT = 1 << 10,
    CHAR_LIMIT = 1 << 8,
    MAX_BUFFER_LINES = 1 << 20,
    LINE_SPACING = 2,
};