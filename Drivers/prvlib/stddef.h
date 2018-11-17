#ifndef _STDDEF_H_
#define _STDDEF_H_

extern int                      *__NULL_PTR_START;

typedef unsigned long	        size_t;

#define NULL                    (void *)&__NULL_PTR_START

#endif
