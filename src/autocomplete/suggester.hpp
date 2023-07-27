#pragma once

#include "cursor.hpp"
#include "rope/rope.hpp"

#include "raylib.h"

#include <cstddef>
#include <string>

class Suggester {
public:
    static constexpr std::size_t max_displayed_keywords = 10;

    void load_text(const std::string& path);
    void load_text(const Rope& path);
    void set_pattern(const std::string& pattern);

    void render(Vector2 origin);
    void to_render(bool to_render);
    bool rendering() const;

    void move_next();
    void move_prev();

    std::string select() const;
    void mark_update();
    bool should_update() const;

private:
    struct ScoredMatch {
        std::size_t kw_index;
        int score;
    };

    Rope m_text{};
    std::string m_pattern{};
    bool m_rendering{};
    std::vector<std::string> m_keywords{};
    std::vector<ScoredMatch> m_matches{};
    std::size_t m_selected{};
    bool m_should_update{true};

    static int calc_score(const std::string& keyword,
                          const std::string& pattern);
};