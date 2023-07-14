#pragma once

#include "buffer.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

class Editor {
public:
    void render();
    void open(std::string_view filename);

private:
    std::vector<Buffer> m_buffers{};
    std::size_t m_buffer_id{};
};