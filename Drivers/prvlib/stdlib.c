//mem.c
//standard memory functions to avoid using incompatible libraries on ARM

#include <FreeRTOS.h>
#include "prvlib/stdlib.h"

void *calloc(size_t num, size_t size) {
    int *p = malloc(num*size);
    
    while(*p++ = 0);
    return p;
}

void *realloc(void *ptr, size_t new_size) {
    free(ptr);
    ptr = malloc(new_size);
    return ptr;
}

//this is not random at all
int next = 1;
int rand(){return (int)((next = next * 1103515245 + 12345) % ((unsigned long)32767 + 1));}
