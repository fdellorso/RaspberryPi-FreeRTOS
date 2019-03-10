/**
 *	Quick and very Dirty STDIO API.
 *
 **/

#include "stdio.h"

#include <rpi_logger.h>

#include <uspi/string.h>
#include <uspi/util.h>

int vsnprintf(char *s, size_t n, const char *format, va_list ap) {
    (void)s;	// FIXME Wunused

    char *truncated;

    TString *message = (TString *) malloc (sizeof (TString));
	String(message);
	StringFormatV (message, format, ap);
    truncated = (char *) StringGet(message);
    _String(message);

    truncated[n] = 0;
    s = truncated;

	println(truncated);

    return (int) strlen(truncated);
}

int sprintf(char *s, const char *format, ...) {
    int strLength = 0;

    va_list var;
	va_start (var, format);

	TString Message;
	String(&Message);
	StringFormatV(&Message, format, var);

    strcpy(s, StringGet(&Message));
    strLength = (int) strlen(StringGet(&Message));
    _String(&Message);
	va_end (var);

    return strLength;
}
