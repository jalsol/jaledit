#include "shared_ptr.h"

#include <stddef.h>
#include <stdlib.h>

struct SharedPtr {
    void *ptr;
    size_t *ref_count;
};

SharedPtr *shared_ptr_new(void *ptr) {
    SharedPtr *shared_ptr = malloc(sizeof(SharedPtr));
    shared_ptr->ptr = ptr;
    shared_ptr->ref_count = malloc(sizeof(size_t));
    *shared_ptr->ref_count = 1;
    return shared_ptr;
}

void shared_ptr_delete(SharedPtr *shared_ptr) {
    if (*shared_ptr->ref_count == 1) {
        free(shared_ptr->ptr);
        free(shared_ptr->ref_count);
        free(shared_ptr);
    } else {
        (*shared_ptr->ref_count)--;
    }
}

void *shared_ptr_get(SharedPtr *shared_ptr) { return shared_ptr->ptr; }

SharedPtr *shared_ptr_clone(SharedPtr *shared_ptr) {
    (*shared_ptr->ref_count)++;
    return shared_ptr;
}