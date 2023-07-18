#pragma once

#include "buffer.hpp"
#include "keybind/keybind.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

enum class EditorMode {
    Normal,
    Insert,
    Command,
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
    EditorMode m_mode{EditorMode::Normal};
    keybind::Trie m_keybinds;

    Buffer& current_buffer();
    const Buffer& current_buffer() const;

    void set_mode(EditorMode mode);
    void normal_mode(char c);
    void insert_mode(char c);
};