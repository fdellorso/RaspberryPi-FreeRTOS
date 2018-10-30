#ifndef _STRING_H
#define _STRING_H

#include "bcm2835_base.h"

char *strcat(char *dest, char *src);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *pString1, const char *pString2);
size_t strlen(const char *s);

void *memset(void *s, int c, size_t n);
void *memcpy2(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#define memcpy(dest, src, size)   memcpy2(dest, src, size)


#endif