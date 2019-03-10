#include "tic_internal.h"

#include <prvlib/stdlib.h>
#include <prvlib/null.h>
#include <uspi/util.h>

char *strdup(const char *pSrc) {
    int len = strlen(pSrc) + 1;

    char *s = malloc(len);

    if (s == NULL)
	{
		return NULL;
	}

    return (char *) memcpy(s, pSrc, len);
}
