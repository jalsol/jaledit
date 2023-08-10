#pragma once

#include <string>
#include <vector>

#include "buffer.hpp"
#include "rope/rope.hpp"

enum class FinderMode {
    None,
    Find,
    Replace,
};

class Finder {
public:
    void find_in_content(const Rope& text);
    [[nodiscard]] Rope replace_in_content(const Rope& text);
    Cursor next_match(Cursor current) const;
    Cursor prev_match(Cursor current) const;
    const std::vector<Cursor>& matches() const;
    const std::vector<std::size_t>& match_idx() const;

    bool is_active() const;
    void render();
    void toggle_prompt(FinderMode mode);
    FinderMode mode() const;
    void switch_buffer();
    void append_char(char c);
    void delete_char();

    Rope& pattern();
    const Rope& pattern() const;

    Rope& replacement();
    const Rope& replacement() const;

    void set_to_highlight(bool to_highlight);
    bool to_highlight() const;

private:
    Rope m_pattern{};
    Rope m_replacement{};
    std::vector<Cursor> m_matches{};
    std::vector<std::size_t> m_match_idx{};

    FinderMode m_mode{};
    Rope* m_active_rope{};
    bool m_to_highlight{};
};