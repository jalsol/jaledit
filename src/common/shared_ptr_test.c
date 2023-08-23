#include "shared_ptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    SharedPtr *sp = shared_ptr_new(malloc(sizeof(char) * 6));
    strcpy((char *)shared_ptr_get(sp), "hello\0");

    SharedPtr *sp2 = shared_ptr_clone(sp);
    SharedPtr *sp3 = shared_ptr_clone(sp);

    printf("string: %s\n", (char *)shared_ptr_get(sp));
    printf("string: %s\n", (char *)shared_ptr_get(sp2));
    printf("string: %s\n", (char *)shared_ptr_get(sp3));

    shared_ptr_delete(sp);
    printf("string: %s\n", (char *)shared_ptr_get(sp2));
    printf("string: %s\n", (char *)shared_ptr_get(sp3));

    strcpy((char *)shared_ptr_get(sp2), "world\0");
    printf("string: %s\n", (char *)shared_ptr_get(sp2));
    printf("string: %s\n", (char *)shared_ptr_get(sp3));

    shared_ptr_delete(sp2);
    printf("string: %s\n", (char *)shared_ptr_get(sp3));

    shared_ptr_delete(sp3);
}