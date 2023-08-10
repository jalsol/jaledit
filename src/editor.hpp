#pragma once

#include "buffer.hpp"
#include "finder/finder.hpp"
#include "keybind/keybind.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

enum class EditorMode {
    Normal,
    Insert,
    Visual,
    BufferList,
    Finder,
};

struct Key {
    int modifier{};
    int key{};
};

class Editor {
public:
    Editor();
    Editor(std::string_view filename);

    void render();

    void update();
    void open(std::string_view filename);

private:
    std::vector<Buffer> m_buffers{};
    std::size_t m_buffer_id{};
    std::size_t m_prev_buffer_id{};
    EditorMode m_mode{EditorMode::Normal};
    Keybind m_keybinds;
    Finder m_finder;

    Buffer& current_buffer();
    const Buffer& current_buffer() const;

    void set_mode(EditorMode mode);
    void reset_to_normal_mode();
    void normal_mode(Key key);
    void insert_mode(Key key);
    void visual_mode(Key key);
    void buffer_list_mode(Key key);
    void finder_mode(Key key);

    void undo();
    void redo();

    void render_buffer();
    void render_status_bar();
    void render_buffer_list();

    void open_file_dialog();
};