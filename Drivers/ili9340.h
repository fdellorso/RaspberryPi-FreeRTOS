#ifndef _ILI9340_H_
#define _ILI9340_H_

#include "prv_types.h"

#define  ILI9340_TFTWIDTH    240
#define  ILI9340_TFTHEIGHT   320

#define  ILI9340_NOP         0x00
#define  ILI9340_SWRESET     0x01
#define  ILI9340_RDDID       0x04
#define  ILI9340_RDDST       0x09

#define  ILI9340_SLPIN       0x10
#define  ILI9340_SLPOUT      0x11
#define  ILI9340_PTLON       0x12
#define  ILI9340_NORON       0x13

#define  ILI9340_RDMODE      0x0A
#define  ILI9340_RDMADCTL    0x0B
#define  ILI9340_RDPIXFMT    0x0C
#define  ILI9340_RDIMGFMT    0x0A
#define  ILI9340_RDSELFDIAG  0x0F

#define  ILI9340_INVOFF      0x20
#define  ILI9340_INVON       0x21
#define  ILI9340_GAMMASET    0x26
#define  ILI9340_DISPOFF     0x28
#define  ILI9340_DISPON      0x29

#define  ILI9340_CASET       0x2A
#define  ILI9340_PASET       0x2B
#define  ILI9340_RAMWR       0x2C
#define  ILI9340_RAMRD       0x2E

#define  ILI9340_PTLAR       0x30
#define  ILI9340_MADCTL      0x36

#define  ILI9340_MADCTL_MY   0x80
#define  ILI9340_MADCTL_MX   0x40
#define  ILI9340_MADCTL_MV   0x20
#define  ILI9340_MADCTL_ML   0x10
#define  ILI9340_MADCTL_RGB  0x00
#define  ILI9340_MADCTL_BGR  0x08
#define  ILI9340_MADCTL_MH   0x04

#define  ILI9340_PIXFMT      0x3A

#define  ILI9340_FRMCTR1     0xB1
#define  ILI9340_FRMCTR2     0xB2
#define  ILI9340_FRMCTR3     0xB3
#define  ILI9340_INVCTR      0xB4
#define  ILI9340_DFUNCTR     0xB6

#define  ILI9340_PWCTR1      0xC0
#define  ILI9340_PWCTR2      0xC1
#define  ILI9340_PWCTR3      0xC2
#define  ILI9340_PWCTR4      0xC3
#define  ILI9340_PWCTR5      0xC4
#define  ILI9340_VMCTR1      0xC5
#define  ILI9340_VMCTR2      0xC7

#define  ILI9340_RDID1       0xDA
#define  ILI9340_RDID2       0xDB
#define  ILI9340_RDID3       0xDC
#define  ILI9340_RDID4       0xDD

#define  ILI9340_GMCTRP1     0xE0
#define  ILI9340_GMCTRN1     0xE1

// #define ILI9340_PWCTR6       0xFC

// Color definitions
#define	ILI9340_BLACK        0x0000
#define	ILI9340_BLUE         0x001F
#define	ILI9340_RED          0xF800
#define	ILI9340_GREEN        0x07E0
#define ILI9340_CYAN         0x07FF
#define ILI9340_MAGENTA      0xF81F
#define ILI9340_YELLOW       0xFFE0  
#define ILI9340_WHITE        0xFFFF
#define ILI9340_GREY         0xAD33

// Waveshare 3.2" COMMAND GPIO Pins
#define TOUCH_IRQ            17
#define DATA_COMMAND         22
#define LCD_RESET            27

// Waveshare 3.2" KEYS GPIO Pins
#define KEY1                 18
#define KEY2                 23
#define KEY3                 24

// char loaded;
uint16_t width, height;

extern  void        ili9340_write_command   (uint8_t command, int param_len, ...);
extern  void        ili9340_init            (void);
extern  void        ili9340_close           (void);
extern  void        ili9340_set_addr_window (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern  void        ili9340_draw_pixel      (uint16_t x, uint16_t y, uint16_t color);
        void        ili9340_draw_char       (unsigned char c, uint16_t x, uint16_t y, uint16_t color);
        void        ili9340_draw_string     (const char* str, uint16_t x, uint16_t y, uint16_t color);
        void        ili9340_println         (const char* message, uint16_t color);
        void        ili9340_printHex        (const char* message, uint32_t hexi, uint16_t color);
extern  void        ili9340_fill_rect       (uint16_t x, uint16_t y, uint16_t  w, uint16_t h, uint16_t color);  
extern  void        ili9340_draw_line_v     (uint16_t x, uint16_t y, uint16_t  h, uint16_t color);  
extern  void        ili9340_draw_line_h     (uint16_t x, uint16_t y, uint16_t  w, uint16_t color);  
extern  void        ili9340_set_rotation    (uint8_t m);       
extern  uint16_t    ili9340_get_width       ();              
extern  uint16_t    ili9340_get_height      ();             
extern  void        ili9340_update_display  ();
        void        ili9340_mkdirty         (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

#endif

// #ifndef _VIDEO_H_
// #define println(message, color)         ili9340_println(message, color)
// #define printHex(message, hexi, color)  ili9340_printHex(message, hexi, color)
// #endif
