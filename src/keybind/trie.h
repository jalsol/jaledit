#pragma once

#include <stdbool.h>
#include <stddef.h>

struct Keybind;
typedef struct Keybind Keybind;

Keybind *keybind_new();
void keybind_delete(Keybind *keybind);

void keybind_add(Keybind *keybind, const char *key, size_t len, bool editable,
                 void (*func)(void));

void keybind_step(Keybind *keybind, char c, bool editable);
void keybind_reset(Keybind *keybind);