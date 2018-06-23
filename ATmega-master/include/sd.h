/*
 * SD.h
 *
 *  Created on: Apr 5, 2012
 *      Author: Clark Scheff
 */

#ifndef SD_H_
#define SD_H_

#include <inttypes.h>

#define SD_RESET		0x00
#define SD_INIT			0x01
#define SD_IF_COND		0x08
#define SD_SEND_CSD		0x09
#define SD_SEND_CID 	0x0A
#define SD_STOP_TX		0x0C
#define SD_BLOCK_LEN	0x10
#define SD_READ_BLOCK	0x11
#define SD_READ_BLOCKS	0x12
#define SD_WRITE_BLOCK	0x18
#define SD_WRITE_BLOCKS	0x19
#define SD_APP_CMD		0x37
#define SD_READ_OCR		0x3A
#define SD_APP_OP		0x29

#define SD_CRC_CMD0		0x95
#define SD_CRC_CMD8		0x87

#define SD_ERROR_NONE			0x00
#define SD_ERROR_TIMEOUT		0xAA
#define SD_ERROR_NOT_SDHC		0xBB
#define SD_ERROR_TIMEOUT_OP		0xCC
#define SD_ERROR_TIMEOUT_OCR	0xDD
#define SD_ERROR_TIMEOUT_READ	0xEE
#define SD_ERROR_TIMEOUT_WRITE	0x42

#define SD_START_BLOCK_TOKEN	0xFE

volatile char sd_buffer[512];

int SD_Init( void );
int SDHC_Init( void );
int SD_Command( char cmd, uint32_t arg);
uint32_t SD_ReadOCR( void );
uint16_t SD_ReadBlock(uint32_t startBlock);
uint16_t SD_WriteBlock(uint32_t startBlock, uint8_t *data, uint16_t size);

#endif /* SD_H_ */
