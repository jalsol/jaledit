#include "autocomplete/suggester.hpp"

#include "constants.hpp"
#include "raylib.h"
#include "rope/rope.hpp"
#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <string>

void Suggester::load_text(const std::string& path) { load_text(Rope{path}); }

void Suggester::load_text(const Rope& path) {
    m_text = path;
    m_keywords.clear();

    for (std::size_t line = 0; line < m_text.line_count(); ++line) {
        const auto line_text = m_text.substr(m_text.find_line_start(line),
                                             m_text.line_length(line));
        std::string word;

        for (std::size_t i = 0; i < line_text.size(); ++i) {
            if (!utils::is_symbol(line_text[i])) {
                continue;
            }

            word.clear();
            std::size_t j = i + 1;
            while (j < line_text.size() && utils::is_symbol(line_text[j])) {
                ++j;
            }

            word = line_text.substr(i, j - i);
            m_keywords.push_back(word);
            i = j;
        }
    }

    std::sort(m_keywords.begin(), m_keywords.end());
    m_keywords.erase(std::unique(m_keywords.begin(), m_keywords.end()),
                     m_keywords.end());

    m_should_update = false;
}

void Suggester::set_pattern(const std::string& pattern) {
    m_pattern = pattern;
    m_matches.clear();

    if (m_pattern.empty()) {
        return;
    }

    for (std::size_t i = 0; i < m_keywords.size(); ++i) {
        int score = calc_score(m_keywords[i], m_pattern);

        if (score > 0) {
            m_matches.push_back({i, score});
        }
    }

    std::sort(
        m_matches.begin(), m_matches.end(),
        [](const auto& lhs, const auto& rhs) { return lhs.score > rhs.score; });
}

void Suggester::render(Vector2 origin) {
    if (!m_rendering) {
        return;
    }

    const Vector2 char_size = utils::measure_text(" ", constants::font_size, 0);
    std::size_t rendered_items
        = std::min(m_matches.size(), max_displayed_keywords);

    std::size_t max_char_size = 0;

    for (std::size_t i = 0; i < rendered_items; ++i) {
        const auto& match = m_matches[i];
        const auto& keyword = m_keywords[match.kw_index];
        max_char_size = std::max(max_char_size, keyword.size());
    }

    // render the background
    DrawRectangle(origin.x, origin.y, char_size.x * max_char_size,
                  char_size.y * rendered_items, {156, 160, 176, 255});

    for (std::size_t i = 0; i < rendered_items; ++i) {
        const auto& match = m_matches[i];
        const auto& keyword = m_keywords[match.kw_index];

        const auto x = origin.x;
        const auto y = origin.y + i * char_size.y;

        if (i == m_selected) {
            DrawRectangle(x, y, char_size.x * keyword.size(), char_size.y,
                          {124, 127, 147, 255});
        }

        utils::draw_text(keyword.c_str(), {x, y}, {230, 233, 239, 255},
                         constants::font_size, 0);
    }
}

void Suggester::move_next() {
    if (m_matches.empty()) {
        return;
    }

    if (!m_rendering) {
        m_rendering = true;
        return;
    }

    std::size_t rendered_items
        = std::min(m_matches.size(), max_displayed_keywords);

    m_selected = (m_selected + 1) % rendered_items;
}

void Suggester::move_prev() {
    if (m_matches.empty()) {
        return;
    }

    if (!m_rendering) {
        m_rendering = true;
        return;
    }

    std::size_t rendered_items
        = std::min(m_matches.size(), max_displayed_keywords);

    m_selected = (m_selected + rendered_items - 1) % rendered_items;
}

std::string Suggester::select() const {
    if (m_matches.empty()) {
        return "";
    }

    return m_keywords[m_matches[m_selected].kw_index];
}

void Suggester::mark_update() { m_should_update = true; }

bool Suggester::should_update() const { return m_should_update; }

void Suggester::to_render(bool to_render) {
    m_rendering = to_render;
    m_selected = 0;
}

bool Suggester::rendering() const { return m_rendering; }

int Suggester::calc_score(const std::string& keyword,
                          const std::string& pattern) {
    // Smith-Waterman algorithm
    // https://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm

    constexpr int match = 16;
    constexpr int mismatch = -128;
    constexpr int gap = -64;

    int max_score = 0;

    std::vector matrix(pattern.size() + 1, std::vector(keyword.size() + 1, 0));

    for (std::size_t i = 1; i <= pattern.size(); ++i) {
        for (std::size_t j = 1; j <= keyword.size(); ++j) {
            int match_score
                = pattern[i - 1] == keyword[j - 1] ? match : mismatch;

            matrix[i][j]
                = std::max({matrix[i - 1][j - 1] + match_score,
                            matrix[i - 1][j] + gap, matrix[i][j - 1] + gap, 0});

            max_score = std::max(max_score, matrix[i][j]);
        }
    }

    return max_score;
}
