#pragma once

#include <climits>

namespace constants::window {

constexpr int width = 1024;
constexpr int height = 768;

} // namespace constants::window

namespace constants::editor {

constexpr int row = 50;
constexpr int col = 85;

} // namespace constants::editor

namespace constants {

constexpr int font_size = 20;
constexpr int margin = 20;
constexpr int buffer_limit = 1 << 10;
constexpr int char_limit = 1 << 8;
constexpr int max_buffer_lines = 1 << 20;
constexpr int line_spacing = 2;
constexpr int max_line_length = INT_MAX / 4 * 3;

} // namespace constants
