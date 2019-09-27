/*
 * ssd1306.c
 *
 * Created: 27.09.2019 18:30:02
 *  Author: Trait
 */ 


#include "ssd1306.h"
#include "ascii_font_6x8.h"
#include "../i2c/i2cmaster.h"

#define SSD1306_ADDR  0x78

//display setup
#define ROWS 8
#define COLS 128

//i2c command value for mode enable: 
#define CMD 0x00
#define DATA 0x40

void ssd1306_run_cmd(void){
	
	i2c_start_wait(SSD1306_ADDR+I2C_WRITE);
	i2c_write(CMD);
}

void ssd1306_run_data(void){
	
	i2c_start_wait(SSD1306_ADDR+I2C_WRITE);
	i2c_write(DATA);
}

void ssd1306_init(void){
	
	i2c_init();
	ssd1306_run_cmd();
	
	//Set MUX ratio
	i2c_write(0xA8); 
	i2c_write(0x3F);
	
	//Set display offset
	i2c_write(0xD3); 
	i2c_write(0x00);
	
	//Set display start line
	i2c_write(0x40); 
	
	//Set segment re-map or 0xA1
	i2c_write(0xA1); 
	
	//Set COM output scan direction or 0xC8 //start from bottom or top
	i2c_write(0xC8); 
	
	//Set COM pins hardware config
	i2c_write(0xDA); 
	i2c_write(0x10);
	
	//Set contrast control
	i2c_write(0x81); 
	i2c_write(0x7F);
	
	//Disable entire display on
	i2c_write(0xA4); 
	
	//Set normal display
	i2c_write(0xA6); 
	
	//Set oscillator frequency
	i2c_write(0xD5); 
	i2c_write(0x80);
	
	//Enable charge pump regulator
	i2c_write(0x8D); 
	i2c_write(0x14);
	
	//Set memory addressing mode to horizontal
	i2c_write(0x20);
	i2c_write(0x00);
	
	//Set start and end column address
	i2c_write(0x21);
	i2c_write(0x00); //start
	i2c_write(0x7F); //end
	
	//Set start and end page address
	i2c_write(0x22);
	i2c_write(0x00); //start
	i2c_write(0x07); //end
	
	//Display on
	i2c_write(0xAF); 
	
	i2c_stop();
}

void ssd1306_goto(uint8_t col, uint8_t page){
	
	ssd1306_run_cmd();
	i2c_write( 0xB0 | (page & 0x07));
	i2c_write(col & 0x0F);
	i2c_write((col>>4 & 0x07) | 0x10);
	i2c_stop();
}

void draw_sign6x8(const int8_t asciiSign){
	
	ssd1306_run_data();
	
	uint8_t i;
	for(i = 0; i < 6; i++){
		if(asciiSign >= ' ' || asciiSign <= 'z'){
			uint16_t charIndex = (asciiSign - ' ')*6;
			i2c_write(ascii_font6x8[charIndex+i]);
		}
	}
	i2c_stop();
}

void draw_text6x8(const char* text, bool alignCenter){
	
	uint8_t textSize = 0;
	uint8_t posX = 0;
	
	while (text[textSize])
		textSize++;
		
	if(alignCenter){
		posX = 64 - (textSize-1)*3;
		ssd1306_goto(posX, 4);
	}
	
	for (uint8_t i = 0; i<textSize; i++){
		draw_sign6x8(text[i]);
	}
}

void ssd1306_clr_scrn(void){
	
	ssd1306_run_data();
	
	uint16_t j;
	for (j = 0; j < 128*8; j++){
		i2c_write(0x00);
	}
	i2c_stop();
}