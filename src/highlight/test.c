#include "lexer.h"
#include "token.h"

#include <stdio.h>

int main() {
    Lexer *lexer = lexer_new("int main() {\n"
                             "    printf(\"Hello, world!\\n\");\n"
                             "    return 0;\n"
                             "}\n");

    Token token;
    while ((token = lexer_next(lexer)).kind != TOKEN_END) {
        if (token.kind == TOKEN_INVALID) {
            continue;
        }

        Color color = token_kind_color(token.kind);

        printf("%s: \"%.*s\"\n", token_kind_name(token.kind), (int)token.length,
               token.start);
        printf("Color is %d, %d, %d\n", color.r, color.g, color.b);
    }

    lexer_delete(lexer);
}