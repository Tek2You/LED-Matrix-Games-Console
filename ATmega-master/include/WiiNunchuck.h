/*
 * WiiNunchuck.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Clark Scheff
 */

#ifndef WIINUNCHUCK_H_
#define WIINUNCHUCK_H_

#define WII_ADDR	0xA4

#define ERROR_BAD_START		0xAA
#define ERROR_NO_SLAVE_ACK	0xBB
#define ERROR_NO_DATA_ACK	0xCC
#define ERROR_NO_DATA_NACK	0xDD

struct button_state_s {
	uint8_t z_pressed:1;
	uint8_t c_pressed:1;
	uint8_t accel_x_lsb:2;
	uint8_t accel_y_lsb:2;
	uint8_t accel_z_lsb:2;
};

/**
 * Nunchuck data comes in a 6 byte packet with the following format
 *  -----------------------------------------------
 * | Byte | Desc                                   |
 *  -----------------------------------------------
 * |   0  | X-axis value for analog stick          |
 * |   1  | Y-axis value for analog stick          |
 * |   2  | X-axis acceleration                    |
 * |   3  | Y-axis acceleration                    |
 * |   4  | Z-axis acceleration                    |
 * |   5  | Buttons and LSB of acceleration values |
 *  -----------------------------------------------
 *
 *  For more info see http://www.windmeadow.com/node/42
 */

typedef struct nunchuk_data_s {
	uint8_t stick_x;
	uint8_t stick_y;
	uint8_t accel_x;
	uint8_t accel_y;
	uint8_t accel_z;
	union {
		uint8_t raw;
		struct button_state_s bits;
	} buttons;
} nunchuck_data_t;

int Wii_Init( void );
int Wii_Data( nunchuck_data_t *data);

#endif /* WIINUNCHUCK_H_ */
