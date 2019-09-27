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
	
	char buffer[5];
	uint8_t counter = 100;
	
    while (1) 
    {
		counter--;
		
		ssd1306_clr_scrn();
		sprintf(buffer,"%d",counter);
		
		draw_text6x8(buffer, true);
		
		_delay_ms(1000);
    }
}

