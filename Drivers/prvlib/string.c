/**
 *	Quick and very Dirty STRING API.
 *
 **/

#include <prvlib/string.h>
#include <prvlib/stdlib.h>

// STRING PART
char *strcat(char *dest, char *src) {
    char *ret = dest;
    while(*ret++);
    while(*ret) {
		*ret++ = *src++;
	}
    *ret = '\0';  
    return dest;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while(*ret) {
		*ret++ = *src++;
	}
    *ret = '\0';
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    do {
        if (!n--) return ret;
		*dest++ = *src++;
    } while (dest);
    while (n--) *dest++ = 0;
    return ret;
}

char *strdup(const char *src) {
    size_t len = strlen(src) + 1;
    char *s = malloc(len);
    if (s == NULL)
        return NULL;
    return (char *)memcpy2(s, src, len);
}

size_t strlen(const char *s) {
    size_t i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

int strcmp(const char *pString1, const char *pString2) {
	while (   *pString1 != '\0'
	       && *pString2 != '\0')
	{
		if (*pString1 > *pString2)
		{
			return 1;
		}
		else if (*pString1 < *pString2)
		{
			return -1;
		}

		pString1++;
		pString2++;
	}

	if (*pString1 > *pString2)
	{
		return 1;
	}
	else if (*pString1 < *pString2)
	{
		return -1;
	}

	return 0;
}

// char *strchr(const char *s, int c) {
//     do {
//         if (*s == c) return (char*)s;
//     } while (*s++);
//     return (0);
// }

// unsigned long strtoul(const char *nptr, char **endptr, register int base)
// {
//     register const char *s = nptr;
//     register unsigned long acc;
//     register int c;
//     register unsigned long cutoff;
//     register int neg = 0, any, cutlim;

// 	/*
// 	 * See strtol for comments as to the logic used.
// 	 */
// 	do {
// 		c = *s++;
// 	} while (ISSPACE(c));
// 	if (c == '-') {
// 		neg = 1;
// 		c = *s++;
// 	} else if (c == '+')
// 		c = *s++;
// 	if ((base == 0 || base == 16) &&
// 	    c == '0' && (*s == 'x' || *s == 'X')) {
// 		c = s[1];
// 		s += 2;
// 		base = 16;
// 	}
// 	if (base == 0) base = c == '0' ? 8 : 10;
// 	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
// 	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
// 	for (acc = 0, any = 0;; c = *s++) {
// 		if (ISDIGIT(c)) c -= '0';
// 		else if (ISALPHA(c)) c -= ISUPPER(c) ? 'A' - 10 : 'a' - 10;
// 		else break;
// 		if (c >= base) break;
// 		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) any = -1;
// 		else {
// 			any = 1;
// 			acc *= base;
// 			acc += c;
// 		}
// 	}
// 	if (any < 0) {
// 		acc = ULONG_MAX;
// 		// errno = ERANGE;
// 	} else if (neg)
// 		acc = -acc;
// 	if (endptr != 0)
// 		*endptr = (char *) (any ? s - 1 : nptr);
// 	return (acc);
// }


// MEM PART
void *memset(void *s, int c, size_t n) {
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}

void *memcpy2(void *dest, const void *src, size_t n) {
	/*unaligned word moves don't work
	for sizes not a multiple of 4,
	this function does not work
	just use a loop inline*/
    char *destp = dest;
    const char *srcp = src;
    while (n--) *destp++ = *srcp++;
	return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char tmp[n];
    memcpy2(tmp,src,n);
    memcpy2(dest,tmp,n);
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    while(n--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            p1++,p2++;
    return 0;
}
