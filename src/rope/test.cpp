#include "rope/rope.hpp"

#include <iostream>

int main() {
    Rope rope("asdfasdf\n");
    std::cout << rope << std::endl;
    std::cout << rope.line_count() << std::endl;
    std::cout << rope.find_line_start(1) << std::endl;

    rope = rope.insert(3, "j");

    std::cout << rope << std::endl;
    std::cout << rope.line_count() << std::endl;
    std::cout << rope.find_line_start(1) << std::endl;
}