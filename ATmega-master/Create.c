/*
 * Create.c
 *
 *  Created on: Apr 4, 2012
 *      Author: Clark Scheff
 */


#include <avr/io.h>
#include <inttypes.h>
#include "include/Create.h"
#include "include/uart.h"

void Create_Init( void )
{
	uartSendByte(COMMAND_START);
	uartSendByte(COMMAND_FULL);
	Create_SetHorn();
}


void Create_Drive( int16_t velocity, int16_t radius)
{
	uartSendByte(COMMAND_DRIVE);
	uartSendByte((char)(velocity >> 8));
	uartSendByte((char)(velocity));
	uartSendByte((char)(radius >> 8));
	uartSendByte((char)(radius));
}


void Create_DriveDirect( int16_t rightVelocity, int16_t leftVelocity)
{
	uartSendByte(COMMAND_DRIVE_DIRECT);
	uartSendByte((char)(rightVelocity >> 8));
	uartSendByte((char)(rightVelocity));
	uartSendByte((char)(leftVelocity >> 8));
	uartSendByte((char)(leftVelocity));
}

void Create_SetHorn()
{
	uartSendByte(COMMAND_SONG);
	uartSendByte(0);
	uartSendByte(1);
	uartSendByte(81);
	uartSendByte(24);
}

void Create_PlayHorn()
{
	uartSendByte(COMMAND_PLAY);
	uartSendByte(0);
}
