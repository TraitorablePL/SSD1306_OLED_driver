/*
 * SSD1306_OLED_driver.c
 *
 * Created: 27.09.2019 17:55:54
 * Author : Trait
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "ssd1306/ssd1306.h"

int main(void)
{
	//Changing clock from 16M to 4M
	CLKPR |= (1<<CLKPCE);
	CLKPR |= (1<<CLKPS1);
	
	ssd1306_init();
	
    while (1) 
    {
		_delay_ms(10);
    }
}

