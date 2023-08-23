#include "vector.h"
#include <stdio.h>

void print_vec(Vec_char *vec) {
    printf("vec: ");
    for (size_t i = 0; i < vec_char_size(vec); i++) {
        printf("%c", vec_char_at(vec, i));
    }
    printf("\n");
}

int main(void) {
    Vec_char *vec = vec_char_new();
    vec_char_push_back(vec, 'a');
    vec_char_push_back(vec, 'b');
    vec_char_push_back(vec, 'c');

    print_vec(vec);

    vec_char_pop_back(vec);

    print_vec(vec);
    printf("back: %c\n", vec_char_back(vec));
    printf("at: %c\n", vec_char_at(vec, 0));
    printf("size: %zu\n", vec_char_size(vec));
    printf("capacity: %zu\n", vec_char_capacity(vec));

    vec_char_push_back(vec, '\0');
    printf("data: %s\n", vec_char_data(vec));

    vec_char_set(vec, 0, 'd');
    print_vec(vec);

    vec_char_reserve(vec, 10);
    printf("capacity: %zu\n", vec_char_capacity(vec));

    vec_char_resize(vec, 5);
    printf("size: %zu\n", vec_char_size(vec));

    Vec_char *subvec = vec_char_new_subvec(vec, 1, 3);
    print_vec(subvec);

    vec_char_delete(vec);
    vec_char_delete(subvec);

    vec = vec_char_new();
    vec_char_push_back(vec, 'a');
    vec_char_push_back(vec, 'b');
    vec_char_push_back(vec, 'c');

    Vec_char *vec2 = vec_char_new_subvec(vec, 0, 3);
    print_vec(vec2);

    vec_char_delete(vec);
    vec_char_delete(vec2);
}