#pragma once

#include <stddef.h>
#include <stdlib.h>

#define DECLARE_VEC(T)                                                                   \
    struct Vec_##T;                                                                      \
    typedef struct Vec_##T Vec_##T;                                                      \
                                                                                         \
    struct Vec_##T *vec_##T##_new();                                                     \
    void vec_##T##_delete(struct Vec_##T *vec);                                          \
    void vec_##T##_push_back(struct Vec_##T *vec, T value);                              \
    void vec_##T##_pop_back(struct Vec_##T *vec);                                        \
    T vec_##T##_back(struct Vec_##T *vec);                                               \
    T vec_##T##_at(struct Vec_##T *vec, size_t index);                                   \
    size_t vec_##T##_size(struct Vec_##T *vec);                                          \
    size_t vec_##T##_capacity(struct Vec_##T *vec);                                      \
    T *vec_##T##_data(struct Vec_##T *vec);                                              \
    void vec_##T##_set(struct Vec_##T *vec, size_t index, T value);                      \
    void vec_##T##_reserve(struct Vec_##T *vec, size_t capacity);                        \
    void vec_##T##_resize(struct Vec_##T *vec, size_t size);                             \
    struct Vec_##T *vec_##T##_new_subvec(struct Vec_##T *vec, size_t start, size_t end);

DECLARE_VEC(char)
DECLARE_VEC(int)
DECLARE_VEC(size_t)

struct RopeNode;
typedef struct RopeNode RopeNode;
typedef RopeNode *RopeNodePtr;
DECLARE_VEC(RopeNodePtr)

#undef DECLARE_VEC