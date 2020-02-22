/*
 * ssd1306.c
 *
 * Created: 27.09.2019 18:30:02
 *  Author: Trait
 */ 


#include "ssd1306.h"
#include "ascii_font_6x8.h"
#include "ascii_font_8x16.h"
#include "../i2c/i2c.h"

//#define SSD1306_ADDR  0x78 //shifted
#define SSD1306_ADDR  0x3C

//display setup
#define ROWS 8
#define COLS 128

//i2c command value for mode enable: 
#define CMD 0x00
#define DATA 0x40

#define BUFF_SIZE 130 // Could be not enough

char TxDataBuffer[BUFF_SIZE];
char RxDataBuffer[BUFF_SIZE];

static struct Cursor cursorParams;
static struct I2C_ActionParams actionParams = {};
	

void ssd1306_cmd_mode(void){
	while(i2c_eGetActionStatus() != I2C_Idle){};
	actionParams.receiverAddr = SSD1306_ADDR;
	actionParams.pTxDataBuffer[0] = CMD;
	actionParams.txBufferLength = 1;
	actionParams.rxBufferLength = 0;
	actionParams.eCurrentAction = I2C_WriteAction;
}

void ssd1306_data_mode(void){
	while(i2c_eGetActionStatus() != I2C_Idle){};
	actionParams.receiverAddr = SSD1306_ADDR;
	actionParams.pTxDataBuffer[0] = DATA;
	actionParams.txBufferLength = 1;
	actionParams.rxBufferLength = 0;
	actionParams.eCurrentAction = I2C_WriteAction;
}

uint8_t ssd1306_send_byte(const char cDataByte){
	
	if (actionParams.txBufferLength != BUFF_SIZE){
		actionParams.pTxDataBuffer[actionParams.txBufferLength] = cDataByte;
		actionParams.txBufferLength++;
		return 1;
	}
	else
		return 0;
}

void ssd1306_init(){
	
	actionParams.pRxDataBuffer = (char*) calloc(BUFF_SIZE, sizeof(char)); //RxDataBuffer;
	actionParams.pTxDataBuffer = (char*) calloc(BUFF_SIZE, sizeof(char)); 
	
	i2c_init();
	ssd1306_cmd_mode();
	
	//Set MUX ratio
	ssd1306_send_byte(0xA8); 
	ssd1306_send_byte(0x3F);
	
	//Set display offset
	ssd1306_send_byte(0xD3); 
	ssd1306_send_byte(0x00);
	
	//Set display start line
	ssd1306_send_byte(0x40); 
	
	//Set segment re-map or 0xA1
	ssd1306_send_byte(0xA1); 
	
	//Set COM output scan direction or 0xC8 //start from bottom or top
	ssd1306_send_byte(0xC8); 
	
	//Set COM pins hardware config
	ssd1306_send_byte(0xDA); 
	ssd1306_send_byte(0x10);
	
	//Set contrast control
	ssd1306_send_byte(0x81); 
	ssd1306_send_byte(0xFF);
	
	//Disable entire display on
	ssd1306_send_byte(0xA4); 
	
	//Set normal display
	ssd1306_send_byte(0xA6); 
	
	//Set oscillator frequency
	ssd1306_send_byte(0xD5); 
	ssd1306_send_byte(0x80);
	
	//Enable charge pump regulator
	ssd1306_send_byte(0x8D); 
	ssd1306_send_byte(0x14);
	
	//Set memory addressing mode to horizontal
	ssd1306_send_byte(0x20);
	ssd1306_send_byte(0x10);		//page
	//ssd1306_send_byte(0x00);		//horizontal
	
	//Set start and end column address
	ssd1306_send_byte(0x21);
	ssd1306_send_byte(0x00);		//start
	ssd1306_send_byte(0x7F);		//end
	
	//Set start and end page address
	ssd1306_send_byte(0x22);
	ssd1306_send_byte(0x00);		//start
	ssd1306_send_byte(0x07);		//end
	
	//Display on
	ssd1306_send_byte(0xAF); 
	
	i2c_proceedToAction(actionParams);
}

void ssd1306_set_page(const uint8_t page){
	
	if(page<8){
		cursorParams.posY = page;
		ssd1306_cmd_mode();
		ssd1306_send_byte( 0xB0 | (page & 0x07));
		i2c_proceedToAction(actionParams);
	}
}

void ssd1306_set_col(const uint8_t col){
	
	if(col<128){
		cursorParams.posX = col;
		ssd1306_cmd_mode();
		ssd1306_send_byte(col & 0x0F);
		ssd1306_send_byte((col>>4 & 0x07) | 0x10);
		i2c_proceedToAction(actionParams);
	}
}

void ssd1306_goto(uint8_t col, uint8_t page){
	
	ssd1306_set_page(page);
	ssd1306_set_col(col);
}
/*
if(asciiSign >= ' ' || asciiSign <= 'z'){
		
		uint16_t charIndex = (asciiSign - ' ')*16;
		uint8_t startPosX = cursor.posX;
		uint8_t i;
		
		for (i = 0; i < 2; i++){
			
			uint8_t j;
		
			ssd1306_goto(startPosX,cursor.posY+i);
			ssd1306_run_data();
		
			for(j = 0; j < 8; j++){
				i2c_write(ascii_font8x16[charIndex+j+(8*i)]);
				cursor.posX++;
				cursor.posX %= 128;
			}
			i2c_stop();
		}
		ssd1306_set_page(cursor.posY-1);
	}
*/

void draw_sign6x8(const int8_t asciiSign){
	
	if(asciiSign >= ' ' || asciiSign <= 'z'){
		
		uint16_t charIndex = (asciiSign - ' ')*6;
		uint8_t i;
		
		ssd1306_goto(cursorParams.posX,cursorParams.posY);
		ssd1306_data_mode();
	
		for(i = 0; i < 6; i++){
			ssd1306_send_byte(pgm_read_byte(&ascii_font6x8[charIndex+i]));
			cursorParams.posX++;
			cursorParams.posX %= 128;
		}
		i2c_proceedToAction(actionParams);
	}
}

void draw_text6x8(const char* text, bool alignCenter){
	
	uint8_t textSize = 0;
	uint8_t startPosX = 0;
	
	while (text[textSize])
		textSize++;
		
	if(alignCenter){
		startPosX = 61 - ((textSize)*6)/2;
		ssd1306_goto(startPosX, cursorParams.posY);
	}
	
	for (uint8_t i = 0; i<textSize; i++){
		draw_sign6x8(text[i]);
	}
}

void draw_sign8x16(const int8_t asciiSign){
	
	if(asciiSign >= ' ' || asciiSign <= 'z'){
		
		uint16_t charIndex = (asciiSign - ' ')*16;
		uint8_t startPosX = cursorParams.posX;
		uint8_t i;
		
		for (i = 0; i < 2; i++){
			
			uint8_t j;
		
			ssd1306_goto(startPosX,cursorParams.posY+i);
			ssd1306_data_mode();
		
			for(j = 0; j < 8; j++){
				ssd1306_send_byte(pgm_read_byte(&ascii_font8x16[charIndex+j+(8*i)]));
				cursorParams.posX++;
				cursorParams.posX %= 128;
			}
			i2c_proceedToAction(actionParams);
		}
		ssd1306_set_page(cursorParams.posY-1);
	}
}

void draw_text8x16(const char* text, bool alignCenter){
	
	uint8_t textSize = 0;
	uint8_t startPosX = 0;
	
	while (text[textSize]){
		textSize++;
	}
	
	if(alignCenter){
		startPosX = 64 - ((textSize)*8)/2;
		ssd1306_goto(startPosX, cursorParams.posY);
	}
	
	for (uint8_t i = 0; i<textSize; i++){
		draw_sign8x16(text[i]);
	}
}

//With current solution sending 128 bytes in one transmission 
//requires space in memory for buffering those 128 bytes

//Buffer could be shrinked by reworking function below 
//and dividing 8 by 128 byte segments into 32 by 32

void ssd1306_clr_scrn(void){
	
	uint8_t i, j;
	
	for (i = 0; i<8; i++)
	{
		ssd1306_goto(0,i);
		ssd1306_data_mode();
		for (j = 0; j < 128; j++){
			ssd1306_send_byte(0x00);
		}
	}
	i2c_proceedToAction(actionParams);
}