#ifndef _STDLIB_H
#define _STDLIB_H

#include "bcm2835_base.h"

void *malloc(unsigned nSize);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t new_size);
void free(void* pBlock);
int rand();

#endif