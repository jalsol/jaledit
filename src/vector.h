#pragma once

#include <stdlib.h>

typedef struct Vector {
    int *data;
    size_t size;
    size_t capacity;
} Vector;

Vector vector_construct();
void vector_destruct(Vector *vector);
void vector_reserve(Vector **vector, size_t new_capacity);
void vector_push_back(Vector *vector, int elem);
size_t vector_lower_bound(Vector *vector, int elem);