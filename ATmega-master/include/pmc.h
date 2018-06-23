/*
 * smc05a.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Clark Scheff
 *
 *  Driver for Pololu dual serial motor controllers
 *  SMC02B SMC035A
 */

#ifndef SMC05A_H_
#define SMC05A_H_

#define START_BYTE 		0x80
#define CONFIG_CHANGE 	0x02
#define FORWARD			0x01
#define REVERSE			0x00


int smc_configure(uint8_t numMotors, uint8_t motorId);
int smc_move(uint8_t motorId, uint8_t direction, uint8_t speed);

#endif /* SMC05A_H_ */
