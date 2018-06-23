/*
 * servo.c
 *
 *  Created on: Apr 3, 2012
 *      Author: Clark Scheff
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include "include/servo.h"
#include "include/uart.h"

typedef struct
{
	volatile uint8_t* PORT;
	uint8_t	MASK;
	uint16_t POSITION;
} servo;

volatile servo servos[NUM_SERVOS];
volatile uint16_t frame_counter;

void Servo_SystemInit( void )
{
	TCNT = 96;
	// enable timer overflow interrupt
	TIMSK |= (1 << TOIE);
	// set prescaler to F_CPU/8 for 8MHz clock
	TCCR = (1 << CS00);
	sei();
}

void Servo_Init( uint8_t servoID, volatile uint8_t *ddr, volatile uint8_t *port, uint8_t mask )
{
	if (servoID >= NUM_SERVOS)
		return;

	*ddr |= (1 << mask);
	*port |= (1 << mask);
	servos[servoID].PORT = port;
	servos[servoID].MASK = mask;
	servos[servoID].POSITION = CENTER_SERVO;
}


void Servo_Position( uint8_t servoID, uint16_t position)
{
	if (position < MIN_SERVO)
		position = MIN_SERVO;
	else if(position > MAX_SERVO)
		position = MAX_SERVO;

	servos[servoID].POSITION = position;
}

ISR(OVF_vect)
{
	uint8_t i;
	TCNT = 96;
	frame_counter++;
	if (frame_counter == MAX_FRAMES)
	{
		//uartSendByte('!');
		frame_counter = 0;
		for (i = 0; i < NUM_SERVOS; i++)
		{
			*(servos[i].PORT) |= (1 << servos[i].MASK);
		}
	}
	else if (frame_counter >= MIN_SERVO)
	{
		for (i = 0; i < NUM_SERVOS; i++)
		{
			if (frame_counter == servos[i].POSITION)
				*(servos[i].PORT) &= ~(1 << servos[i].MASK);
		}
	}
}
