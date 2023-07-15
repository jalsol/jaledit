#pragma once

#include "rope/rope.hpp"

#include <array>
#include <cstddef>

std::size_t fib(std::size_t n);

consteval std::array<std::size_t, Rope::max_depth> fib_list();