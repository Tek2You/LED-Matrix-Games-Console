/*
 * WiiNunchuck.c
 *
 *  Created on: Apr 3, 2012
 *      Author: Clark Scheff
 */
#include "include/i2c.h"
#include "include/WiiNunchuck.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define ANALOG_X_CENTER 0x80
#define ANALOG_Y_CENTER 0x80


int Wii_Init( void )
{
	i2c_start();	// send start
	i2c_sendByte(WII_ADDR);
	i2c_sendByte(0xF0);
	i2c_sendByte(0x55);
	i2c_stop();

	_delay_ms(1);
	i2c_start();	// send start
	i2c_sendByte(WII_ADDR);
	i2c_sendByte(0xFB);
	i2c_sendByte(0x00);
	i2c_stop();

	return 0;
}


int Wii_Data( nunchuck_data_t *data )
{
	int i = 0x21;

	if (data == 0)
		return -1;

	// send 0x00 to let nunchuck know we are going to request data
	i2c_start();	// send start
	i2c_sendByte(WII_ADDR);
	i2c_sendByte(0x00);
	i2c_stop();

	// switch to read mode by adding 1 to WII_ADDR
	_delay_ms(1);
	i2c_start();	// send start
	i2c_sendByte(WII_ADDR | 1);
	uint8_t *packet = (uint8_t*)data;
	for(i = 0; i < 6; i++)
	{
		if(i < 5)
		{
			i2c_ack();
		}
		else
		{
			i2c_nack();
		}

		*(packet+i) = i2c_getByte();
	}
	i2c_stop();
	return i;
}
