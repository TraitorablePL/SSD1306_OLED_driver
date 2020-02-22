/*
 * i2c.c
 *
 * Created: 03.02.2020 15:58:22
 *  Author: Daniel Stankiewicz
 */ 

#include "i2c.h"
#include <avr/interrupt.h>

// MCU clock
#define F_CPU 4000000UL //explicit declaration of type Unsigned Long

// I2C Bus clock
#define SCL_CLOCK  100000L //explicit declaration of type Long

// I2C Status
#define READ 1
#define WRITE 0
#define START 0x08
#define REP_START  0x10

// Master Transmitter Mode
#define ADDR_WRITE_ACK 0x18
#define ADDR_WRITE_NACK 0x20
#define WRITE_DATA_ACK 0x28
#define WRITE_DATA_NACK 0x30

// Master Receiver Mode
#define ADDR_READ_ACK 0x40
#define ADDR_READ_NACK 0x48
#define READ_DATA_ACK 0x50
#define READ_DATA_NACK 0x58

// I2C Delimiters
#define POLL_LIMIT 3

static struct I2C_ActionParams localParams;
static uint8_t pollingCounter;

ISR(TWI_vect){
	switch(localParams.eCurrentAction){
		case I2C_WriteAction:
			i2c_writeAction();
			break;
			
		case I2C_ReadAction:
			break;
			
		case I2C_WriteAndReadAction:
			break;
			
		default:
			break;
	}
}

void i2c_init(void){
	sei();
	
	TWSR = 0;
	TWBR = (F_CPU/SCL_CLOCK-16)/2;
	
	TWCR = (1<<TWEN);
	
	localParams.receiverAddr = 0;
	localParams.pRxDataBuffer = 0;
	localParams.pTxDataBuffer = 0;
	localParams.eCurrentAction = I2C_Idle;
}

void i2c_writeAction(void){
	switch(TWSR){
		case START:
			localParams.dataCounter = 0;
			TWDR = (localParams.receiverAddr<<1) | WRITE;
			TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
			break;
		
		case ADDR_WRITE_ACK:
		case WRITE_DATA_ACK:
			pollingCounter = 0;
			if(localParams.dataCounter < localParams.txBufferLength) {
				TWDR = localParams.pTxDataBuffer[localParams.dataCounter];
				TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
				localParams.dataCounter++;
			}
			else {
				TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO);
				localParams.eCurrentAction = I2C_Idle;
			}
			break;
	
		case ADDR_WRITE_NACK:
		case WRITE_DATA_NACK:
			if(pollingCounter < POLL_LIMIT) {
				TWDR = localParams.pTxDataBuffer[localParams.dataCounter-1];
				TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
				pollingCounter++;
			}
			else {
				TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO);
				localParams.eCurrentAction = I2C_Idle;
			}
			break;
	}
}

//TODO
void i2c_readAction(void){
}

//TODO
void i2c_writeAndReadAction(void){
	
}

void i2c_proceedToAction(struct I2C_ActionParams ActionParams){
	
	while(localParams.eCurrentAction != I2C_Idle){}
		
	pollingCounter = 0;
	localParams = ActionParams;
	TWCR = (1<<TWSTA) | (1<<TWIE) | (1<<TWEN) | (1<<TWINT);
}

enum I2C_Actions i2c_eGetActionStatus(void){
	return localParams.eCurrentAction;
}

