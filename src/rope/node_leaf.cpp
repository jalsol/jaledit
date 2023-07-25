#include "rope/node.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace rope {

Leaf::Leaf(const std::string& text) : m_text{text} {
    m_weight = text.length();
    m_length = text.length();

    for (std::size_t i = 0; i < m_length; ++i) {
        if (m_text[i] == '\n') {
            m_lfpos.push_back(i);
        }
    }

    m_lfweight = m_lfcnt = m_lfpos.size();
}

char Leaf::operator[](std::size_t index) const { return m_text[index]; }

std::string Leaf::substr(std::size_t start, std::size_t length) const {
    return m_text.substr(start, length);
}

std::string Leaf::to_string() const { return m_text; }

std::pair<Node::Handle, Node::Handle> Leaf::split(std::size_t index) const {
    return {
        std::make_shared<Leaf>(m_text.substr(0, index)),
        std::make_shared<Leaf>(m_text.substr(index)),
    };
}

std::vector<Node::Handle> Leaf::leaves() const {
    return {std::make_shared<Leaf>(*this)};
}

std::size_t Leaf::find_line_feed(std::size_t index) const {
    return m_lfpos.at(index);
}

} // namespace rope