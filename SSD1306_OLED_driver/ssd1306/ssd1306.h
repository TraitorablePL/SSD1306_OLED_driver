/*
 * ssd1306.h
 *
 * Created: 27.09.2019 18:30:24
 *  Author: Trait
 */ 


#ifndef SSD1306_H_
#define SSD1306_H_

#include <avr/io.h>

void ssd1306_init(void);
void draw_sign6x8(const int8_t asciiSign);


#endif /* SSD1306_H_ */