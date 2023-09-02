#pragma once

#include <stdbool.h>
#include <stddef.h>

struct Keybind;
typedef struct Keybind Keybind;

Keybind *keybind_new();
void keybind_delete(Keybind *keybind);

struct Editor;
typedef struct Editor Editor;
void keybind_add(Keybind *keybind, const char *key, size_t len, bool editable,
                 void (*func)(Editor *editor));

void keybind_step(Keybind *keybind, Editor *editor, char c, bool editable);
void keybind_reset(Keybind *keybind);