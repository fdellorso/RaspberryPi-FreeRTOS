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

    #ifdef VIDEO
		println(truncated, 0xFFFFFFFF);
	#endif
	#ifdef ILI9340
		ili9340_println(truncated, ILI9340_WHITE);
	#endif
	#ifdef MUART
		muart_println(truncated);
	#endif

    return (int)strlen(truncated);

	(void)s;	// FIXME Wunused
}
