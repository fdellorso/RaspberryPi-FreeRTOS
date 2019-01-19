#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <rpi_header.h>
#include <uspi/string.h>

#define VIDEO_AQUA      0xFF00FFFF
#define VIDEO_BLUE      0xFF0088FF
#define VIDEO_GREEN     0xFF00FF00
#define VIDEO_MAGENTA   0xFFFF00FF
#define VIDEO_ORANGE    0xFFFF9900
#define VIDEO_RED       0xFFFF0000
#define VIDEO_VIOLET    0xFFCC88CC
#define VIDEO_WHITE     0xFFFFFFFF


char loaded;

void enablelogging(void);

void video_init(void);

void video_putc(unsigned char c, int x, int y, unsigned int colour);
void video_puts(const char* str, int x, int y, unsigned int colour);
void video_println(const char* message, unsigned int colour);
void video_printHex(const char* message, unsigned int hexi, unsigned int colour);
void video_printf(const char *pMessage, unsigned int colour, ...);
void video_vprintf(const char *pMessage, unsigned int colour, va_list var);

void videotest(void);
void dumpDebug(void);

#endif
