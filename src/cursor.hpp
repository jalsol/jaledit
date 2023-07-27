#pragma once

struct Cursor {
    int line{};
    int column{};

    auto operator<=>(const Cursor&) const = default;
};