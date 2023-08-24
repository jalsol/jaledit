#include "trie.h"

#include <stdio.h>

void hello() { printf("Hello, world!\n"); }

void cruel() { printf("Hello, cruel world!\n"); }

int main() {
    Keybind *keybind = keybind_new();

    keybind_add(keybind, "abc", 3, false, hello);

    keybind_step(keybind, 'a', false);
    keybind_step(keybind, 'b', false);
    keybind_step(keybind, 'c', false);

    keybind_step(keybind, 'a', false);
    keybind_step(keybind, 'b', false);
    keybind_reset(keybind);
    keybind_step(keybind, 'c', false);

    keybind_add(keybind, "cruel", 5, false, cruel);
    keybind_reset(keybind);

    keybind_step(keybind, 'c', false);
    keybind_step(keybind, 'r', false);
    keybind_step(keybind, 'u', false);
    keybind_step(keybind, 'e', false);
    keybind_step(keybind, 'l', false);

    keybind_delete(keybind);
    return 0;
}