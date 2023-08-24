#include "utils.h"

#include <ctype.h>

bool is_symbol_start(char c) { return isalpha(c) || c == '_'; }

bool is_symbol(char c) { return isalnum(c) || c == '_'; }