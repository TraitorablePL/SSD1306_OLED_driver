/*
 * SSD1306_OLED_driver.c
 *
 * Created: 27.09.2019 17:55:54
 * Author : Trait
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>

#include "ssd1306/ssd1306.h"

int main(void)
{
	//Changing clock from 16M to 4M
	CLKPR = (1<<CLKPCE);
	CLKPR = (1<<CLKPS1);
	
	ssd1306_init();
	
	ssd1306_clr_scrn();
	
	ssd1306_set_page(2);
	draw_text8x16("Big Test", true);

	ssd1306_set_page(5);
	draw_text6x8("Cursor at:", true);
	
	char buffer[14];
	
	sprintf(buffer,"Col: %d, Page: %d",0,0);
	
	ssd1306_set_page(7);
	draw_text6x8(buffer, true);
	
	//uint8_t counter = 0;
	
    while (1) {
		/*
		ssd1306_clr_scrn();
		sprintf(buffer,"%d",counter);
		
		draw_text6x8(buffer, true);
		_delay_ms(100);
		counter++;
		*/
    }
}

