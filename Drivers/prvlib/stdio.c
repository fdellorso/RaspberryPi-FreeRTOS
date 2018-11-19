/**
 *	Quick and very Dirty STDIO API.
 *
 **/

#include <prvlib/stdio.h>
#include <prvlib/stdlib.h>
#include <rpi_logger.h>

#include <uspi/string.h>

int vsnprintf(char *s, size_t n, const char *format, va_list ap) {
    char *truncated;

    TString *message = (TString *) malloc (sizeof (TString));
	String(message);
	StringFormatV (message, format, ap);
    truncated = (char *) StringGet(message);
    _String(message);

    truncated[n] = 0;
    s = truncated;

	println(truncated);

    return (int)strlen(truncated);

	(void)s;	// FIXME Wunused
}
