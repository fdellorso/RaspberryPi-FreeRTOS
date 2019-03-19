#ifndef _RPI_LOGGER_H_
#define _RPI_LOGGER_H_

#ifdef VIDEO
	#include <video.h>

	#define LOGGER_BLUE		VIDEO_BLUE
	#define LOGGER_GREEN	VIDEO_GREEN
	#define LOGGER_MAGENTA	VIDEO_MAGENTA
	#define LOGGER_RED		VIDEO_RED
	#define LOGGER_WHITE	VIDEO_WHITE

	#define logger_init		video_init
	#define	putc			video_putc
	#define	puts			video_puts
	#define	println(a)		video_println(a, LOGGER_WHITE)
	#define	printHex(a, b)	video_printHex(a, b, LOGGER_WHITE)
	#define	printf(a, ...)	video_printf(a, LOGGER_WHITE, __VA_ARGS__)
	#define	vprintf(a, ...)	video_vprintf(a, LOGGER_WHITE, __VA_ARGS__)
	#define getc			NOOP
	#define gets			NOOP
#endif
#ifdef ILI9340
	#include <ili9340.h>

	#define	LOGGER_BLUE		ILI9340_BLUE
	#define	LOGGER_GREEN	ILI9340_GREEN
	#define LOGGER_MAGENTA	ILI9340_MAGENTA
	#define	LOGGER_RED		ILI9340_RED
	#define LOGGER_WHITE	ILI9340_WHITE

	#define logger_init		ili9340_init
	#define putc			ili9340_putc
	#define puts			ili9340_puts
	#define println(a)		ili9340_println(a, LOGGER_WHITE)
	#define printhex(a, b)	ili9340_printHex(a, b, LOGGER_WHITE)
	#define printf(a, ...)	ili3940_printf(a, LOGGER_WHITE, __VA_ARGS__)
	#define vprintf(a, ...)	ili3940_vprintf(a, LOGGER_WHITE, __VA_ARGS__)
	#define getc			NOOP
	#define gets			NOOP
#endif
#ifdef MUART
	#include <muart.h>

	#define logger_init		muart_init
	#define	putc			muart_putc
	#define	puts			muart_puts
	#define	println			muart_println
	#define	printHex		muart_printHex
	#define	printf			muart_printf
	#define	vprintf			muart_vprintf
	#define getc			muart_getc
	#define gets			muart_gets
#endif

#endif
