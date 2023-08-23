#pragma once

struct SharedPtr;
typedef struct SharedPtr SharedPtr;

SharedPtr *shared_ptr_new(void *ptr);
void shared_ptr_delete(SharedPtr *shared_ptr);
void *shared_ptr_get(SharedPtr *shared_ptr);
SharedPtr *shared_ptr_clone(SharedPtr *shared_ptr);