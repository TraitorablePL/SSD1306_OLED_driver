/*
 * ssd1306.h
 *
 * Created: 27.09.2019 18:30:24
 *  Author: Trait
 */ 


#ifndef SSD1306_H_
#define SSD1306_H_

#include <avr/io.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

struct Cursor {
	uint8_t posX;
	uint8_t posY;
};

void ssd1306_init(void);
void ssd1306_clr_scrn(void);

void ssd1306_goto(const uint8_t col, const uint8_t page);
void ssd1306_set_page(const uint8_t page);
void ssd1306_set_col(const uint8_t col);

void draw_text6x8(const char* text, bool alignCenter);
void draw_sign6x8(const int8_t asciiSign);
void draw_text8x16(const char* text, bool alignCenter);
void draw_sign8x16(const int8_t asciiSign);

// TO ADD
//getCursorX
//getCursorY

#endif /* SSD1306_H_ */