/*
 * smc05a.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Clark Scheff
 */
#include "include/uart.h"
#include "include/pmc.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>



int smc_configure(uint8_t numMotors, uint8_t motorId) {
	uint8_t data[] = {START_BYTE, CONFIG_CHANGE, numMotors << 6 | motorId};
	uint8_t flag;
	uartSendBuffer(data, 3, &flag);
	return 0;
}

int smc_move(uint8_t motorId, uint8_t direction, uint8_t speed) {
	if (speed > 127) speed = 127;
	uint8_t data[] = {START_BYTE, 0, motorId << 1 | direction, speed};
	uint8_t flag;
	uartSendBuffer(data, 4, &flag);
	return 0;
}
