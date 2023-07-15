#include "rope/utils.hpp"
#include "rope/rope.hpp"

#include <array>
#include <cstddef>

consteval std::array<std::size_t, Rope::max_depth> make_fibs() {
    std::array<std::size_t, Rope::max_depth> result{};
    result[0] = 1;
    result[1] = 1;
    for (std::size_t i = 2; i < Rope::max_depth; ++i) {
        result[i] = result[i - 1] + result[i - 2];
    }
    return result;
}

constexpr std::array<std::size_t, Rope::max_depth> fibs = make_fibs();

std::size_t fib(std::size_t n) { return fibs[n]; }

consteval std::array<std::size_t, Rope::max_depth> fib_list() { return fibs; }