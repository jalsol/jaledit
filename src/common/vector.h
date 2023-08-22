#pragma once

#include <stddef.h>
#include <stdlib.h>

#define DECLARE_VEC(T)                                                                   \
    struct Vec_##T;                                                                      \
    typedef struct Vec_##T Vec_##T;                                                      \
                                                                                         \
    struct Vec_##T *Vec_##T##_new();                                                     \
    void Vec_##T##_delete(struct Vec_##T *vec);                                          \
    void Vec_##T##_push_back(struct Vec_##T *vec, T value);                              \
    void Vec_##T##_pop_back(struct Vec_##T *vec);                                        \
    T Vec_##T##_back(struct Vec_##T *vec);                                               \
    T Vec_##T##_at(struct Vec_##T *vec, size_t index);                                   \
    size_t Vec_##T##_size(struct Vec_##T *vec);                                          \
    size_t Vec_##T##_capacity(struct Vec_##T *vec);                                      \
    T *Vec_##T##_data(struct Vec_##T *vec);                                              \
    void Vec_##T##_set(struct Vec_##T *vec, size_t index, T value);                      \
    void Vec_##T##_reserve(struct Vec_##T *vec, size_t capacity);                        \
    void Vec_##T##_resize(struct Vec_##T *vec, size_t size);                             \
    struct Vec_##T *Vec_##T##_new_subvec(struct Vec_##T *vec, size_t start, size_t end);

DECLARE_VEC(char)
DECLARE_VEC(int)

#undef DECLARE_VEC