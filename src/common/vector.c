#include "vector.h"

#include "utils.h"

#define DEFINE_VEC(T)                                                                    \
    struct Vec_##T {                                                                     \
        size_t capacity;                                                                 \
        size_t size;                                                                     \
        T *data;                                                                         \
    };                                                                                   \
    struct Vec_##T *vec_##T##_new() {                                                    \
        struct Vec_##T *vec = malloc(sizeof(struct Vec_##T));                            \
        vec->capacity = 0;                                                               \
        vec->size = 0;                                                                   \
        vec->data = NULL;                                                                \
        return vec;                                                                      \
    }                                                                                    \
    void vec_##T##_delete(struct Vec_##T *vec) {                                         \
        free(vec->data);                                                                 \
        free(vec);                                                                       \
    }                                                                                    \
    void vec_##T##_push_back(struct Vec_##T *vec, T value) {                             \
        if (vec->size == vec->capacity) {                                                \
            if (vec->capacity == 0) {                                                    \
                vec->capacity = 4;                                                       \
            }                                                                            \
            vec->capacity = vec->capacity * 2;                                           \
            vec->data = realloc(vec->data, vec->capacity * sizeof(T));                   \
        }                                                                                \
        vec->data[vec->size++] = value;                                                  \
    }                                                                                    \
    void vec_##T##_pop_back(struct Vec_##T *vec) { vec->size--; }                        \
    T vec_##T##_back(struct Vec_##T *vec) { return vec->data[vec->size - 1]; }           \
    T vec_##T##_at(struct Vec_##T *vec, size_t index) { return vec->data[index]; }       \
    size_t vec_##T##_size(struct Vec_##T *vec) { return vec->size; }                     \
    size_t vec_##T##_capacity(struct Vec_##T *vec) { return vec->capacity; }             \
    T *vec_##T##_data(struct Vec_##T *vec) { return vec->data; }                         \
    void vec_##T##_set(struct Vec_##T *vec, size_t index, T value) {                     \
        vec->data[index] = value;                                                        \
    }                                                                                    \
    void vec_##T##_reserve(struct Vec_##T *vec, size_t capacity) {                       \
        if (vec->capacity < capacity) {                                                  \
            vec->capacity = capacity;                                                    \
            vec->data = realloc(vec->data, vec->capacity * sizeof(T));                   \
        }                                                                                \
    }                                                                                    \
    void vec_##T##_resize(struct Vec_##T *vec, size_t size) {                            \
        if (vec->capacity < size) {                                                      \
            vec->capacity = size;                                                        \
            vec->data = realloc(vec->data, vec->capacity * sizeof(T));                   \
        }                                                                                \
        vec->size = size;                                                                \
    }                                                                                    \
    struct Vec_##T *vec_##T##_new_subvec(struct Vec_##T *vec, size_t start,              \
                                         size_t end) {                                   \
        end = MIN(end, vec->size);                                                       \
        struct Vec_##T *subvec = vec_##T##_new();                                        \
        vec_##T##_reserve(subvec, end - start + 1);                                      \
        for (size_t i = start; i < end; i++) {                                           \
            vec_##T##_push_back(subvec, vec->data[i]);                                   \
        }                                                                                \
        return subvec;                                                                   \
    }

DEFINE_VEC(char)
DEFINE_VEC(int)
DEFINE_VEC(size_t)

// RopeNodePtr defined in rope/node.h
DEFINE_VEC(RopeNodePtr)