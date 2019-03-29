#include "ili9340.h"
#include "font_5x5.h"

#include <stdlib.h>
#include <uspi/string.h>

#define CHAR_WIDTH 	6
#define CHAR_HEIGHT 8

static char loaded = 0;
static char lcdbuffer[2 * ILI9340_TFTWIDTH * ILI9340_TFTHEIGHT];
static uint32_t width, height;
static uint16_t dirty_x0, dirty_y0, dirty_x1, dirty_y1;
static uint16_t lcd_position_x = 0;
static uint16_t lcd_position_y = 0;

void ili9340_write_command(uint8_t command, int param_len, ...);
void ili9340_update_display(void);
void ili9340_draw_pixel(uint16_t x, uint16_t y, uint16_t colour);
void ili9340_fill_rect(uint16_t x, uint16_t y, uint16_t  w, uint16_t h, uint16_t colour);
void ili9340_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ili9340_mkdirty(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ili9340_draw_line(void);
void ili9340_draw_line_v(uint16_t x, uint16_t y, uint16_t  h, uint16_t colour);  
void ili9340_draw_line_h(uint16_t x, uint16_t y, uint16_t  w, uint16_t colour);  
uint16_t ili9340_get_width(void);              
uint16_t ili9340_get_height(void);             
void ili9340_colour_test(void);


void ili9340_write_command(uint8_t command, int param_len, ...) {
	uint32_t i;
	char buffer[50];
	va_list args;

	// bcm2835_gpio_write(22, LOW);
	SetGpio(DATA_COMMAND, LOW);
	// bcm2835_spi_transfer(command);
	spi0_transfer(command);
	// bcm2835_gpio_write(22, HIGH);
	SetGpio(DATA_COMMAND, HIGH);

	if (param_len) {
		va_start(args, param_len);
		for (i = 0; i < (uint32_t) param_len; i++) {
			buffer[i] = (uint8_t) va_arg(args, int);
		}
		va_end(args);
		// bcm2835_spi_writenb(buffer, param_len);
		spi0_writenb(buffer, param_len);
	}
}

void ili9340_draw_line(void) {
}

void ili9340_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if (x0 >= width) x0 = width - 1;
	if (y0 >= height) y0 = height - 1;
	if (x1 >= width) x1 = width - 1;
	if (y1 >= height) y1 = height - 1;
	if (x0 > x1 || y0 > y1) return;

	ili9340_write_command(ILI9340_CASET, 4, x0 >> 8, x0 & 0xff, x1 >> 8, x1 & 0xff);
	ili9340_write_command(ILI9340_PASET, 4, y0 >> 8, y0 & 0xff, y1 >> 8, y1 & 0xff);

	ili9340_write_command(ILI9340_RAMWR, 0);
}

void ili9340_draw_pixel(uint16_t x, uint16_t y, uint16_t colour) {
	if (x >= width) x = width - 1;
	if (y >= height) y = height - 1;

	ili9340_mkdirty(x, y, x, y);

	uint32_t offset = (y * width + x) << 1;
	lcdbuffer[offset++] = (colour >> 8) & 0xff;
	lcdbuffer[offset] = colour & 0xff;
}

void ili9340_putc(unsigned char c, uint16_t x, uint16_t y, uint16_t colour) {
	uint16_t i, j;

	//convert the character to an index
	c = c & 0x7F;
	if (c < ' ') {
		c = 0;
	} else {
		c -= ' ';
	}

	//draw pixels of the character
	for (j = 0; j < CHAR_WIDTH; j++) {
		for (i = 0; i < CHAR_HEIGHT; i++) {
			//unsigned char temp = font[c][j];
			if (font_5x5[c][j] & (1<<i)) {
				// framebuffer[(y + i) * SCREEN_WIDTH + (x + j)] = colour;
				ili9340_draw_pixel(x + j, y + i, colour);
			}
		}
	}
}

void ili9340_puts(const char* str, uint16_t x, uint16_t y, unsigned short colour) {
	while (*str) {
		ili9340_putc(*str++, x, y, colour);
		x += CHAR_WIDTH; 
	}
}

void ili9340_println(const char* message, unsigned short colour) {
	// int x;

	if(loaded == 0) return; //if video isn't loaded don't bother

	// int nFlags;
	// __asm volatile ("mrs %0, cpsr" : "=r" (nFlags));
	// char s_bWereEnabled = nFlags & 0x80 ? 0 : 1; 
	// if(s_bWereEnabled) __asm volatile ("cpsid i" : : : "memory");

	if(lcd_position_y >= height || lcd_position_y == 0) {
		lcd_position_y = 0;
		ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
	}

	ili9340_puts(message, lcd_position_x, lcd_position_y, colour);
	lcd_position_y = lcd_position_y + CHAR_HEIGHT + 1;

	ili9340_update_display();

	// if(lcd_position_y >= height){
	// 	if(lcd_position_x + 2 * (width / 8) > width){

	// 		volatile int* timeStamp = (int*)0x20003004;
	// 		int stop = *timeStamp + 5000 * 1000;
	// 		while (*timeStamp < stop) __asm__("nop");

	// 		for(x = 0; x < width * height; x++){
	// 			// framebuffer[x] = 0xFF000000;
	// 			lcdbuffer[x++] = (ILI9340_BLACK >> 8) & 0xff;
	// 			lcdbuffer[x] = ILI9340_BLACK & 0xff;
	// 		}
	// 		lcd_position_y = 0;
	// 		lcd_position_x = 0;
	// 	}else{
	// 		lcd_position_y = 0;
	// 		lcd_position_x += width / 8;
	// 	}
	// }

	// if(s_bWereEnabled) __asm volatile ("cpsie i" : : : "memory");
}

void ili9340_printHex(const char* message, unsigned int hexi, unsigned short colour) {
	if(loaded == 0) return; //if video isn't loaded don't bother

	// TODO disabilitata perche usa memcpy della stdlib
	char hex[16] = {'0','1','2','3','4','5','6','7',
					'8','9','A','B','C','D','E','F'};

	// char *hex;
	// memcpy(hex, "0123456789ABCDEF", sizeof("0123456789ABCDEF"));
	
	// char m[200];			// used malloc to reduce stack usage
	char * m = (char *) malloc(sizeof(char) * 200);
	int i = 0;
	while (*message){
		m[i] = *message++;
		i++;
	}
	//overwrite the null terminator
	m[i + 0] = hex[(hexi >> 28)&0xF];
	m[i + 1] = hex[(hexi >> 24)&0xF];
	m[i + 2] = hex[(hexi >> 20)&0xF];
	m[i + 3] = hex[(hexi >> 16)&0xF];
	m[i + 4] = hex[(hexi >> 12)&0xF];
	m[i + 5] = hex[(hexi >> 8)&0xF];
	m[i + 6] = hex[(hexi >> 4)&0xF];
	m[i + 7] = hex[(hexi >> 0)&0xF];
	m[i + 8] = 0; //null termination

	ili9340_println(m, colour);

	free(m);
}

void ili3940_printf(const char *pMessage, unsigned short colour, ...) {
	va_list var;
	va_start (var, pMessage);

	TString Message;
	String(&Message);
	StringFormatV(&Message, pMessage, var);

	ili9340_println(StringGet(&Message), colour);

	_String(&Message);
	va_end (var);
}

void ili3940_vprintf(const char *pMessage, unsigned short colour, va_list var) {
	TString Message;
	String(&Message);
	StringFormatV(&Message, pMessage, var);

	ili9340_println(StringGet(&Message), colour);

	_String(&Message);
}

void ili9340_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour) {
	uint16_t i;

	if (x >= width) x = width - 1;
	if (y >= height) y = height - 1;
	if (x + w > width) w = width - x;
	if (y + h > height) h = height - y;

	ili9340_mkdirty(x, y, x + w - 1, y + h - 1);

	uint8_t hi = (colour >> 8) & 0xff, lo = colour & 0xff;
	uint32_t offset = (y * width + x) << 1;

	while (h--) {
		for (i = w; i; i--) {
			lcdbuffer[offset++] = hi;
			lcdbuffer[offset++] = lo;
		}
		offset += (width - w) << 1;
	}
}

void ili9340_draw_line_v(uint16_t x, uint16_t y, uint16_t h, uint16_t colour) {
	ili9340_fill_rect(x, y, 1, h, colour);
}


void ili9340_draw_line_h(uint16_t x, uint16_t y, uint16_t w, uint16_t colour) {
	ili9340_fill_rect(x, y, w, 1, colour);
}

void ili9340_mkdirty(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	if (x0 < dirty_x0) dirty_x0 = x0;
	if (y0 < dirty_y0) dirty_y0 = y0;
	if (x1 > dirty_x1) dirty_x1 = x1;
	if (y1 > dirty_y1) dirty_y1 = y1;
}

void ili9340_update_display(void) {
	if (dirty_x0 >= width || dirty_x1 >= width ||
			dirty_y0 >= height || dirty_y1 >= height) {
		dirty_x0 = 0;
		dirty_x1 = width - 1;
		dirty_y0 = 0;
		dirty_y1 = height - 1;
	}
	if (dirty_x0 > dirty_x1) {
		dirty_x0 = 0;
		dirty_x1 = width - 1;
	}
	if (dirty_y0 > dirty_y1) {
		dirty_y0 = 0;
		dirty_y1 = height - 1;
	}

	ili9340_set_addr_window(dirty_x0, dirty_y0, dirty_x1, dirty_y1);

	uint32_t offset = 2 * (dirty_y0 * width + dirty_x0);
	uint32_t len = 2 * (dirty_x1 - dirty_x0 + 1);

	while (dirty_y0 <= dirty_y1) {
		// bcm2835_spi_writenb(lcdbuffer + offset, len);
		spi0_writenb(lcdbuffer + offset, len);
		offset += width << 1;
		dirty_y0++;
	}

	dirty_x0 = width;
	dirty_x1 = 0;
	dirty_y0 = height;
	dirty_y1 = 0;
}

uint16_t ili9340_get_width(void) {
	return width;
}

uint16_t ili9340_get_height(void) {
	return height;
}

void ili9340_set_rotation(uint8_t m) {
	uint8_t rotation;
	
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
		case 0:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTWIDTH;
			height = ILI9340_TFTHEIGHT;
			break;
		case 1:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTHEIGHT;
			height = ILI9340_TFTWIDTH;
			break;
		case 2:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTWIDTH;
			height = ILI9340_TFTHEIGHT;
			break;
		case 3:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MV | ILI9340_MADCTL_MY 
					| ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTHEIGHT;
			height = ILI9340_TFTWIDTH;
			break;
		default:
			break;
	}

	dirty_x0 = width;
	dirty_x1 = 0;
	dirty_y0 = height;
	dirty_y1 = 0;
}

void ili9340_init(void) {
	// bcm2835_spi_begin();
	// bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  
	// bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); 
	// bcm2835_spi_chipSelect(BCM2835_SPI_CS0);		// CS Enable
	spi0_init();
	spi0_set_data_mode(SPI0_MODE0);                  
	spi0_set_clock_divider(SPI0_CLOCK_DIVIDER_16); 
	spi0_chip_select(SPI0_CS0);		// CS Enable


	// bcm2835_gpio_fsel(22, BCM2835_GPIO_FSEL_OUTP);	// D/C Pin
	SetGpioFunction(DATA_COMMAND, GPIO_FUNC_OUTPUT);
	// bcm2835_gpio_fsel(27, BCM2835_GPIO_FSEL_OUTP);	// Reset Pin
	SetGpioFunction(LCD_RESET, GPIO_FUNC_OUTPUT);

	// Waveshare 3.2" RESET Init Cycle
	// bcm2835_gpio_write(27, LOW);
	// bcm2835_gpio_write(27, HIGH);
	// bcm2835_delay(5);
	// bcm2835_gpio_write(27, LOW);
	// bcm2835_delay(20);
	// bcm2835_gpio_write(27, HIGH);
	// bcm2835_delay(150);
	SetGpio(LCD_RESET, LOW);
	SetGpio(LCD_RESET, HIGH);
	DelayMilliSysTimer(5);
	SetGpio(LCD_RESET, LOW);
	DelayMilliSysTimer(20);
	SetGpio(LCD_RESET, HIGH);
	DelayMilliSysTimer(150);

	ili9340_write_command(0xEF, 3, 0x03, 0x80, 0x02);
	ili9340_write_command(0xCF, 3, 0x00 , 0XC1 , 0X30);
	ili9340_write_command(0xED, 4, 0x64 , 0x03 , 0X12 , 0X81);
	ili9340_write_command(0xE8, 3, 0x85 , 0x00 , 0x78);
	ili9340_write_command(0xCB, 5, 0x39 , 0x2C , 0x00 , 0x34 , 0x02);
	ili9340_write_command(0xF7, 1, 0x20);
	ili9340_write_command(0xEA, 2, 0x00 , 0x00);

	/* Power Control 1 */
	ili9340_write_command(0xC0, 1, 0x23);

	/* Power Control 2 */
	ili9340_write_command(0xC1, 1, 0x10);

	/* VCOM Control 1 */
	ili9340_write_command(0xC5, 2, 0x3e, 0x28);

	/* VCOM Control 2 */
	ili9340_write_command(0xC7, 1, 0x86);

	/* Memory Access Control */
	ili9340_write_command(0x36, 1, 0x40 | 0x08);

	/* COLMOD: Pixel Format Set */
	/* 16 bits/pixel */
	ili9340_write_command(0x3A, 1, 0x55);

	/* Frame Rate Control */
	/* Division ratio = fosc, Frame Rate = 79Hz */
	ili9340_write_command(0xB1, 2, 0x00, 0x18);

	/* Display Function Control */
	ili9340_write_command(0xB6, 3, 0x08, 0x82, 0x27);

	/* Gamma Function Disable */
	ili9340_write_command(0xF2, 1, 0x00);

	/* Gamma curve selected  */
	ili9340_write_command(0x26, 1, 0x01);

	/* Positive Gamma Correction */
	ili9340_write_command(0xE0, 15,
			0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
			0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00);

	/* Negative Gamma Correction */
	ili9340_write_command(0xE1, 15,
			0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
			0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);

	/* Sleep OUT */
	ili9340_write_command(0x11, 0);

	bcm2835_delay(120);

	/* Display ON */
	ili9340_write_command(0x29, 0);

	ili9340_set_rotation(0);

	dirty_x0 = width;
	dirty_x1 = 0;
	dirty_y0 = height;
	dirty_y1 = 0;

	loaded = 1;
}

void ili9340_close(void) {
	// bcm2835_spi_end();
	spi0_close();
}

void ili9340_colour_test(void) {
	uint16_t wid, hei;

	for(wid = 0; wid < 240; wid++) {
		for(hei = 0; hei < 320; hei++) {
			if((hei&0x60) == 0)
				ili9340_draw_pixel(wid, hei, (hei & 0x1F));
			if((hei&0x60) == 32)
				ili9340_draw_pixel(wid, hei, (hei & 0x1F) << 5);
			if((hei&0x60) == 64)
				ili9340_draw_pixel(wid, hei, (hei & 0x1F) << 11);
			if((hei&0x60) == 96)
				ili9340_draw_pixel(wid, hei, (hei & 0x1F) + ((hei & 0x1F) << 5) +  ((hei & 0x1F) << 11));			
		}
	}

	ili9340_update_display();
}
