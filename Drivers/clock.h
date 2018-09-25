#ifndef _CLOCK_H_
#define _CLOCK_H_

#define CLOCK_WIDTH  40
#define CLOCK_HEIGHT 80

// Valore di intervallo per produrre un Interrupt
// ogni secondo su System Timer
#define ST_INTERVAL 0x000F4240 

uint16_t width, height;
uint8_t hour, minute, second, current_clk;
uint8_t exit_clk;

void ili9340_clock_char(unsigned char c, uint16_t x, uint16_t y, uint16_t color);
void ili9340_clock_string(char *str, uint16_t x, uint16_t y, uint16_t color);
void ili9340_clock_setup();
void ili9340_clock();
char *int2time(char timep[], uint8_t hour, uint8_t minute, uint16_t second);

// char *str_cat(char string1[], char string2[]);
// char *str_cpy(char string1[], char string2[]);

#endif