#include "buffer.hpp"

#include <fstream>
#include <sstream>
#include <string_view>

Buffer::Buffer() : m_rope{"\n"} {}

Buffer::Buffer(std::string_view filename) : m_filename{filename} {
    std::ifstream file{filename.data()};
    if (!file) {
        throw std::runtime_error{"Could not open file"};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_rope = Rope{buffer.str()};
    m_cursor.character = m_rope[0];
}

const Cursor& Buffer::cursor() const { return m_cursor; }

const Rope& Buffer::rope() const { return m_rope; }