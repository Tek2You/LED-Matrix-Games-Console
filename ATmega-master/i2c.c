#include <avr/io.h>
#include "include/i2c.h"

void i2c_init(long fcpu, long fi2c)
{
	// Set prescaler to 1
	TWSR = 0;
	// Set clock to 100 kHz
	TWBR = (fcpu / fi2c - 16) / 2;
	// Set TWI Enable Bits
	TWCR |= (1 << TWEN); 
}

void i2c_start (void)
{
	// Send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait for TWINT flag to be set.
	while(!(TWCR & (1<<TWINT)))
		;
}



void i2c_wait (void)
{
	// wait for TWINT flag to be set.
	while(!(TWCR & (1<<TWINT)))
		;
}



void i2c_sendByte(uint8_t data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for TWINT flag to be set.
	while(!(TWCR & (1<<TWINT)))
		;
}


uint8_t i2c_getStatus(void)
{
	return (TWSR & 0xF8);
}


void i2c_stop (void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}


void i2c_ack (void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	// wait for TWINT flag to be set.
	while(!(TWCR & (1<<TWINT)))
		;
}

void i2c_nack (void)
{
	TWCR = (1<<TWINT) | (1<<TWEN);//Transmit NACK 
	// wait for TWINT flag to be set.
	while(!(TWCR & (1<<TWINT)))
		;
}


uint8_t i2c_getByte (void)
{
	return TWDR;
}

