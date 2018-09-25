/**
 *	Quick and very Dirty CLOCK API.
 *
 **/

#include "bcm2835.h" // da togliere
#include "clock.h"
#include "ili9340.h"
#include "font_clock.h"

#include "prv_types.h"
#include "prv_string.h"

#define KEY_IRQ 1

uint16_t width, height;
uint8_t hour, minute, second, current_clk;
uint8_t exit_clk;

void ili9340_clock_char(unsigned char c, uint16_t x, uint16_t y, uint16_t color) {
	uint16_t i, j;
	uint16_t height_counter;
	const byte_size = 8;
	
	//convert the character to an index
	c = c & 0x7F;
	if (c < ' ') {
		c = 0;
	} else {
		c -= ' ';
	}

	height_counter = CLOCK_HEIGHT / byte_size;

	//draw pixels of the character
	for (j = 0; j < CLOCK_WIDTH; j++) {
		for (i = 0; i < CLOCK_HEIGHT; i++) {
				//unsigned char temp = font[c][j];
				if (font_clock[c][i/byte_size + j*height_counter] & ((1<<7)>>i%byte_size)) {
					// framebuffer[(y + i) * SCREEN_WIDTH + (x + j)] = colour;
					ili9340_draw_pixel(x + j, y + i, color);
				}
		}
	}
}

void ili9340_clock_string(char *str, uint16_t x, uint16_t y, uint16_t color) {
	while (*str) {
		ili9340_clock_char(*str++, x, y, color);
		x += CLOCK_WIDTH; 
	}
}

void ili9340_clock_setup() {
    char time1[9], time2[9];

    hour = 0;
    minute = 0;
    second = 0;

    // Impostazione KEY1,2,3 sul Modulo LCD
    // Pull Up on GPIO 18, 23, 24
    bcm2835_gpio_fsel(KEY1,BCM2835_GPIO_FSEL_INPT); //usare gpio.h
    bcm2835_gpio_fsel(KEY2,BCM2835_GPIO_FSEL_INPT); //usare gpio.h
    bcm2835_gpio_fsel(KEY3,BCM2835_GPIO_FSEL_INPT); //usare gpio.h

    // Function Select Input for GPIO 18, 23, 24
    bcm2835_gpio_set_pud(KEY1,BCM2835_GPIO_PUD_UP); //usare gpio.h
    bcm2835_gpio_set_pud(KEY2,BCM2835_GPIO_PUD_UP); //usare gpio.h
    bcm2835_gpio_set_pud(KEY3,BCM2835_GPIO_PUD_UP); //usare gpio.h

    // Inizializzazione LCD
    // Impostazioni già presenti nel Main.c
    // ili9340_init();
	// ili9340_set_rotation(1);

    // ili9340_fill_rect(0,0,width,height,ILI9340_RED);
    // ili9340_update_display();

    current_clk = 0;
    exit_clk = 1;

    while(exit_clk) {
        int2time(time1, current_clk, minute, second);
        int2time(time2, 99, minute, second);

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
	    ili9340_clock_string(time1, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
	    ili9340_update_display();
        bcm2835_delay(500); //creare bcm2835_base

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
        ili9340_clock_string(time2, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
	    ili9340_update_display();
        bcm2835_delay(500); //creare bcm2835_base

        #if(KEY_IRQ == 0)
            if(bcm2835_gpio_lev(18) == 0) { //usare gpio.h
                if(hour == 23) hour =  0;
                else           hour += 1;
            }

            if(bcm2835_gpio_lev(23) == 0) { //usare gpio.h
                if(hour == 0)  hour =  23;
                else           hour -= 1;
            }

            if(bcm2835_gpio_lev(24) == 0) //usare gpio.h
                exit_clk = 0;
        #endif
    }

    hour = current_clk;

    current_clk = 0;
    exit_clk = 2;

    while(exit_clk) {
        int2time(time1, hour, current_clk, second);
        int2time(time2, hour, 99, second);

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
	    ili9340_clock_string(time1, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
	    ili9340_update_display();
        bcm2835_delay(500); //creare bcm2835_base

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
        ili9340_clock_string(time2, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
	    ili9340_update_display();
        bcm2835_delay(500); //creare bcm2835_base

        #if(KEY_IRQ == 0)
            if(bcm2835_gpio_lev(18) == 0) { //usare gpio.h
                if(minute == 59) minute =  0;
                else             minute += 1;
            }

            if(bcm2835_gpio_lev(23) == 0) { //usare gpio.h
                if(minute == 0)  minute =  59;
                else             minute -= 1;
            }

            if(bcm2835_gpio_lev(24) == 0) //usare gpio.h
                exit_clk = 0;
        #endif
    }

    minute = current_clk;

    current_clk = 0;
    exit_clk = 3;

    while(exit_clk) {
        int2time(time1, hour, minute, current_clk);
        int2time(time2, hour, minute, 99);

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
	    ili9340_clock_string(time1, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
	    ili9340_update_display();
        bcm2835_delay(500); //creare bcm2835_base

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
        ili9340_clock_string(time2, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
	    ili9340_update_display();
        bcm2835_delay(500); //creare bcm2835_base

        #if(KEY_IRQ == 0)
            if(bcm2835_gpio_lev(18) == 0) { //usare gpio.h
                if(second == 59) second =  0;
                else             second += 1;
            }

            if(bcm2835_gpio_lev(23) == 0) { //usare gpio.h
                if(second == 0)  second =  59;
                else             second -= 1;
            }

            if(bcm2835_gpio_lev(24) == 0) //usare gpio.h
                exit_clk = 0;
        #endif
    }

    second = current_clk;
}

void ili9340_clock() {
    char times[9];

    if(second == 59) {
        if(minute == 59) {
            if(hour == 23) {
                hour =  0;
            }
            else {
                hour += 1;
            }

            minute = 0;
        }
        else {
            minute += 1;
        }

        second = 0;
    }
    else {
        second += 1;
    }

    int2time(times, hour, minute, second);
    ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
    ili9340_clock_string(times, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
    ili9340_update_display();
}

void ili9340_clock2() {
    // volatile uint32_t* paddr;
    char times[9];
    uint32_t st;
    uint8_t change = 1;
    

    uint32_t onesec_bit = 0x00100000;

	// paddr = bcm2835_st + BCM2835_ST_CLO/4;
	// st = bcm2835_peri_read(paddr);

    st = (uint32_t)bcm2835_st_read();
    ili9340_printHex("System Timer: ", st, ILI9340_GREEN);
    ili9340_printHex("Gate Counter: ", onesec_bit, ILI9340_GREEN);

    if(st >= onesec_bit) {
        if(second == 59) {
            if(minute == 59) {
                if(hour == 23) {
                    hour =  0;
                }
                else {
                    hour += 1;
                }

                minute = 0;
            }
            else {
                minute += 1;
            }

            second = 0;
        }
        else {
            second += 1;
        }

        change = 1;
        onesec_bit += 0x00100000;
    }

    if(change == 1) {
        int2time(times, hour, minute, second);

        ili9340_fill_rect(0,0,width,height,ILI9340_BLACK);
        ili9340_clock_string(times, (width - 8*CLOCK_WIDTH)/2, (height - CLOCK_HEIGHT)/2 , ILI9340_RED);
        ili9340_update_display();
        change = 0;
    }  
}

char *int2time(char timep[], uint8_t hour, uint8_t minute, uint16_t second) {
    char *conv[60] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59"};
    char *space = "  ";
    char separator[2] = ":";
    char hourp[3], minutep[3], secondp[3];

    if(hour == 99)   str_cpy(space, hourp);
    else             str_cpy(conv[hour], hourp);

    if(minute == 99) str_cpy(space, minutep);
    else             str_cpy(conv[minute], minutep);

    if(second == 99) str_cpy(space, secondp);
    else             str_cpy(conv[second], secondp);

    str_cpy(hourp, timep);
    str_cat(timep, separator);
    str_cat(timep, minutep);
    str_cat(timep, separator);
    str_cat(timep, secondp);

    return timep;
}

/*
char *str_cat(char string1[], char string2[]) {
    uint8_t i, j;

    // calculate the length of string s1
    // and store it in i
    for(i = 0; string1[i] != '\0'; ++i);

    for(j = 0; string2[j] != '\0'; ++j, ++i)
    {
        string1[i] = string2[j];
    }

    string1[i] = '\0';

    return string1;
}

char *str_cpy(char string1[], char string2[]) {
    uint8_t i;
 
    while (string1[i] != '\0') {
        string2[i] = string1[i];
        i++;
    }
 
    string2[i] = '\0';

    return string2;
}
*/