#include "vector.h"
#include <stdio.h>

void print_vec(Vec_char *vec) {
    printf("vec: ");
    for (size_t i = 0; i < Vec_char_size(vec); i++) {
        printf("%c", Vec_char_at(vec, i));
    }
    printf("\n");
}

int main(void) {
    Vec_char *vec = Vec_char_new();
    Vec_char_push_back(vec, 'a');
    Vec_char_push_back(vec, 'b');
    Vec_char_push_back(vec, 'c');

    print_vec(vec);

    Vec_char_pop_back(vec);

    print_vec(vec);
    printf("back: %c\n", Vec_char_back(vec));
    printf("at: %c\n", Vec_char_at(vec, 0));
    printf("size: %zu\n", Vec_char_size(vec));
    printf("capacity: %zu\n", Vec_char_capacity(vec));

    Vec_char_push_back(vec, '\0');
    printf("data: %s\n", Vec_char_data(vec));

    Vec_char_set(vec, 0, 'd');
    print_vec(vec);

    Vec_char_reserve(vec, 10);
    printf("capacity: %zu\n", Vec_char_capacity(vec));

    Vec_char_resize(vec, 5);
    printf("size: %zu\n", Vec_char_size(vec));

    Vec_char *subvec = Vec_char_new_subvec(vec, 1, 3);
    print_vec(subvec);

    Vec_char_delete(vec);
    Vec_char_delete(subvec);
}