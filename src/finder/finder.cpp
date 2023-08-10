#include "finder/finder.hpp"

#include "constants.hpp"
#include "rope/rope.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <vector>

// void Finder::set_pattern(const Rope& pattern) { m_pattern = pattern; }

Rope& Finder::pattern() { return m_pattern; }

const Rope& Finder::pattern() const { return m_pattern; }

Rope& Finder::replacement() { return m_replacement; }

const Rope& Finder::replacement() const { return m_replacement; }

void Finder::switch_buffer() {
    if (m_mode == FinderMode::None) {
        if (m_active_rope == nullptr) {
            m_active_rope = &m_pattern;
        }
        return;
    }

    if (m_active_rope == &m_pattern) {
        m_active_rope = &m_replacement;
    } else {
        m_active_rope = &m_pattern;
    }
}

void Finder::append_char(char c) {
    if (m_active_rope == nullptr) {
        m_active_rope = &m_pattern;
    }

    *m_active_rope = m_active_rope->append(std::string(1, c));
}

void Finder::delete_char() {
    if (m_active_rope == nullptr) {
        m_active_rope = &m_pattern;
    }

    if (m_active_rope->length() == 0) {
        return;
    }

    *m_active_rope = m_active_rope->erase(m_active_rope->length() - 1, 1);
}

void Finder::find_in_content(const Rope& text) {
    m_matches.clear();
    m_match_idx.clear();

    if (m_pattern.length() == 0) {
        return;
    }

    Rope z_text = m_pattern.append("$").append(text);
    std::size_t z_length = z_text.length();
    std::vector<std::size_t> z(z_length, 0);
    std::size_t l = 0, r = 0;

    for (std::size_t i = 1; i < z_length; i++) {
        if (i < r) {
            z[i] = ((r - i) < z[i - l]) ? (r - i) : z[i - l];
        }
        while (i + z[i] < z_length && z_text[z[i]] == z_text[i + z[i]]) {
            ++z[i];
        }
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }

    Cursor cursor{};
    for (std::size_t i = m_pattern.length() + 1; i < z_length; i++) {
        char c = z_text[i];
        if (z[i] == m_pattern.length()) {
            m_matches.push_back(cursor);
            m_match_idx.push_back(i - m_pattern.length() - 1);
        }

        if (c == '\n') {
            cursor.column = 0;
            ++cursor.line;
        } else {
            ++cursor.column;
        }
    }
}

Rope Finder::replace_in_content(const Rope& text) {
    find_in_content(text);
    Rope ret = text;

    std::for_each(m_match_idx.rbegin(), m_match_idx.rend(), [&](auto i) {
        ret = ret.erase(i, m_pattern.length()).insert(i, m_replacement);
    });

    return ret;
}

Cursor Finder::next_match(Cursor current) const {
    if (m_matches.empty()) {
        return current;
    }

    auto it = std::upper_bound(m_matches.begin(), m_matches.end(), current);
    if (it == m_matches.end()) {
        return m_matches.front();
    }

    return *it;
}

Cursor Finder::prev_match(Cursor current) const {
    if (m_matches.empty()) {
        return current;
    }

    auto it = std::lower_bound(m_matches.begin(), m_matches.end(), current);
    if (it == m_matches.begin()) {
        return m_matches.back();
    }

    return *std::prev(it);
}

const std::vector<Cursor>& Finder::matches() const { return m_matches; }

const std::vector<std::size_t>& Finder::match_idx() const {
    return m_match_idx;
}

bool Finder::is_active() const { return m_mode != FinderMode::None; }

FinderMode Finder::mode() const { return m_mode; }

void Finder::toggle_prompt(FinderMode mode) {
    if (m_mode == mode) {
        m_mode = FinderMode::None;
    } else {
        m_mode = mode;
    }

    if (m_active_rope == nullptr) {
        m_active_rope = &m_pattern;
    }

    if (m_mode == FinderMode::Find) {
        m_active_rope = &m_pattern;
    }
}

void Finder::render() {
    if (m_mode == FinderMode::None) {
        return;
    }

    constexpr float margin = constants::margin * 10.F;

    const Rectangle container = {
        margin,
        margin,
        GetScreenWidth() - 2 * margin,
        GetScreenHeight() - 3 * margin,
    };

    DrawRectangleRec(container, {188, 192, 204, 255});

    const Rectangle find_input_box = {
        container.x + constants::margin * 3 + 20,
        container.y + constants::margin,
        container.width - constants::margin * 5,
        22,
    };

    constexpr Color inactive_bg = {255, 255, 255, 255};
    constexpr Color active_bg = {220, 224, 232, 255};

    utils::draw_text("Find ", {find_input_box.x - 70, find_input_box.y}, BLACK,
                     20, 0);
    if (m_active_rope == &m_pattern) {
        DrawRectangleRec(find_input_box, active_bg);
    } else {
        DrawRectangleRec(find_input_box, inactive_bg);
    }

    const Vector2 char_size = utils::measure_text(" ", 20, 0);
    const std::size_t input_len = find_input_box.width / char_size.x;

    std::string pattern = m_pattern.substr(0, input_len);
    utils::draw_text(pattern.c_str(), {find_input_box.x, find_input_box.y},
                     BLACK, 20, 0);

    // draw cursor
    const Vector2 find_cursor_pos = {
        find_input_box.x + char_size.x * pattern.length(),
        find_input_box.y,
    };

    DrawRectangle(find_cursor_pos.x, find_cursor_pos.y, 2, char_size.y, BLACK);

    if (m_mode == FinderMode::Find) {
        return;
    }

    const Rectangle replace_input_box = {
        find_input_box.x,
        container.y + constants::margin * 3 + find_input_box.height,
        find_input_box.width,
        find_input_box.height,
    };

    utils::draw_text("Replace ",
                     {replace_input_box.x - 70, replace_input_box.y}, BLACK, 20,
                     0);
    if (m_active_rope == &m_replacement) {
        DrawRectangleRec(replace_input_box, active_bg);
    } else {
        DrawRectangleRec(replace_input_box, inactive_bg);
    }

    const std::size_t replace_len = replace_input_box.width / char_size.x;
    std::string replace = m_replacement.substr(0, replace_len);

    utils::draw_text(replace.c_str(),
                     {replace_input_box.x, replace_input_box.y}, BLACK, 20, 0);

    // draw cursor
    const Vector2 replace_cursor_pos = {
        replace_input_box.x + char_size.x * replace.length(),
        replace_input_box.y,
    };

    DrawRectangle(replace_cursor_pos.x, replace_cursor_pos.y, 2, char_size.y,
                  BLACK);
}

void Finder::set_to_highlight(bool to_highlight) {
    m_to_highlight = to_highlight;
}

bool Finder::to_highlight() const { return m_to_highlight; }