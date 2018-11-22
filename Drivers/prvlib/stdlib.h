#ifndef _STDLIB_H
#define _STDLIB_H

#include <prvlib/stddef.h>

void *malloc(unsigned nSize);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t new_size);
void free(void* pBlock);

int abs(int n);
int atoi(char * str);
int rand(void);

#endif