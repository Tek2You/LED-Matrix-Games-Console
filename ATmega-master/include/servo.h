/*
 * servo.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Clark Scheff
 */

#ifndef SERVO_H_
#define SERVO_H_

#define SERVO_DDR	DDRB
#define SERVO_PORT	PORTB
#define SERVO_PIN1	PINB1
#define SERVO_PIN2	PINB2
#define MAX_FRAMES	1000

#if defined(__AVR_ATmega88__)
#define TCCR		TCCR0B
#define TCNT		TCNT0
#define TIMSK		TIMSK0
#define TIFR		TIFR0
#define TOIE		TOIE0
#define OVF_vect	TIMER0_OVF_vect
#elif defined(__AVR_ATmega8__)
#define TCCR		TCCR0
#define TCNT		TCNT0
#define TIMSK		TIMSK
#define TIFR		TIFR
#define TOIE		TOIE
#define OVF_vect	TIMER0_OVF_vect
#endif

#define MIN_SERVO 		20
#define MAX_SERVO		95
#define CENTER_SERVO	(MIN_SERVO + MAX_SERVO) >> 1

#define NUM_SERVOS		4

void Servo_SystemInit( void );
void Servo_Init( uint8_t servoID, volatile uint8_t *ddr, volatile uint8_t *port, uint8_t mask );
void Servo_Position( uint8_t servoID, uint16_t position);

#endif /* SERVO_H_ */
