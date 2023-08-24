#include "suggester.h"

#include <stdio.h>

int main() {
    Suggester *suggester = Suggester_new();
    Rope *text = rope_new_from_text("hello world\n", 12);
    suggester_load_text(suggester, text);
    suggester_set_pattern(suggester, "h", 1);

    printf("suggestion: %s\n", suggester_get_selected(suggester));

    suggester_delete(suggester);
    rope_delete(text);
}