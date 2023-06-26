#include "vector.h"

#include <stdlib.h>

Vector vector_construct() {
    Vector vector;
    vector.size = 0;
    vector.capacity = 64;
    vector.data = calloc(vector.capacity, sizeof(int));
    return vector;
}

void vector_destruct(Vector *vector) {
    vector->size = 0;
    vector->capacity = 0;
    free(vector->data);
    vector->data = NULL;
}

void vector_reserve(Vector **vector, size_t new_capacity) {
    if (new_capacity <= (*vector)->capacity) {
        return;
    }
    (*vector)->data = realloc((*vector)->data, new_capacity * sizeof(int));
    (*vector)->capacity = new_capacity;
}

void vector_push_back(Vector *vector, int elem) {
    if (vector->size == vector->capacity) {
        vector_reserve(&vector, 2 * vector->capacity);
    }

    vector->data[vector->size++] = elem;
}

size_t vector_lower_bound(Vector *vector, int elem) {
    size_t low = 0;
    size_t high = vector->size;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        if (vector->data[mid] < elem) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    return low;
}