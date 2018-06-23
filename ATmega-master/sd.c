/*
 * SD.c
 *
 *  Created on: Apr 5, 2012
 *      Author: Clark Scheff
 */

#include "include/sd.h"
#include "include/spi.h"
#include <avr/io.h>
#include <avr/delay.h>


int SD_Init( void )
{
	uint8_t i = 0;
	uint16_t timeout = 0;
	uint8_t spcr;

	spcr = SPI_SPCR;
	// set SPI clck to 125kHz (8MHz / 64)
	SPI_SPCR = ( 1 << SPI_SPE ) | ( 1 << SPI_MSTR ) | ( 1 << SPI_SPR1 );

	// set DI and CS high and apply >= 74 SCLK pulses
	SPI_DeassertSS();
	for (i = 0; i < 10; i++)
		SPI_SendByte(0xFF);

	SPI_AssertSS();
	while (SD_Command(SD_RESET, 0) != 1 && timeout < 0x7FFF)
		timeout++;

	SPI_SPCR = spcr;

	if (timeout == 0x7FFF)
		return -1;

	return 0;
}


int SDHC_Init( void )
{
	uint8_t i = 0, resp;
	uint16_t timeout = 0;
	uint8_t spcr;

	spcr = SPI_SPCR;
	// set SPI clck to 125kHz (8MHz / 64)
	SPI_SPCR = ( 1 << SPI_SPE ) | ( 1 << SPI_MSTR ) | ( 1 << SPI_SPR1 );

	// set MOSI and SS high and apply >= 74 SCLK pulses
	SPI_DeassertSS();
	for (i = 0; i < 10; i++)
		SPI_SendByte(0xFF);

	SPI_AssertSS();
	do
	{
		resp = SD_Command(SD_RESET, 0);
		if (timeout++ > 0x20)
			return SD_ERROR_TIMEOUT;
	} while (resp != 1);

	SPI_DeassertSS();

	SPI_SPCR = spcr;

	timeout = 0;
	do
	{
		resp = SD_Command(SD_IF_COND, 0x000001AA);	// only for SDHC cards
		if(timeout++ > 0xFE)
			return SD_ERROR_NOT_SDHC;

	} while (resp != 1);

	timeout = 0;
	do
	{
		resp = SD_Command(SD_APP_CMD, 0);
		resp = SD_Command(SD_APP_OP, 0x40000000);
		if(timeout++ > 0xFE)
			return SD_ERROR_TIMEOUT_OP;

	} while (resp != 0);

	timeout = 0;
	do
	{
		resp = SD_Command(SD_READ_OCR, 0);
		if (timeout++ > 0xFE)
			return SD_ERROR_TIMEOUT_OCR;
	} while (resp != 0);

	return SD_ERROR_NONE;
}


int SD_Command( char cmd, uint32_t arg)
{
	uint8_t resp, count = 0;

	SPI_AssertSS();
	// all commands should start with 01 so OR cmd with 0x40
	SPI_SendByte(cmd | 0x40);
	SPI_SendByte((uint8_t)(arg >> 24));
	SPI_SendByte((uint8_t)(arg >> 16));
	SPI_SendByte((uint8_t)(arg >> 8));
	SPI_SendByte((uint8_t)(arg));

	// send CRC
	if (cmd == SD_IF_COND)
		SPI_SendByte(SD_CRC_CMD8);
	else
		SPI_SendByte(SD_CRC_CMD0);

	// wait for MISO to go low
	while ((resp = SPI_SendByte(0xFF)) == 0xFF)
	{
		if(count++ > 0xFE)
			break;
	}

	if (resp == 0 && cmd == SD_READ_OCR)
	{
		if ( (SPI_SendByte(0xFF) & 0x40) != 0x40 )
			resp = 1;
		SPI_SendByte(0xFF);
		SPI_SendByte(0xFF);
		SPI_SendByte(0xFF);
	}

	// send 8 clocks to get card ready for next command
	SPI_SendByte(0xFF);
	SPI_DeassertSS();
	return resp;
}


uint16_t SD_ReadBlock(uint32_t startBlock)
{
	uint8_t resp;
	uint16_t i, timeout = 0;

	resp = SD_Command(SD_READ_BLOCK, startBlock);
	if (resp != 0)
		return resp;

	SPI_AssertSS();
	while (SPI_ReadByte() != SD_START_BLOCK_TOKEN)
		if (timeout++ > 0xFFFE)
		{
			SPI_DeassertSS();
			return SD_ERROR_TIMEOUT_READ;
		}

	for(i = 0; i < 512; i++)
		sd_buffer[i] = SPI_ReadByte();

	// read CRC out (unused)
	SPI_ReadByte();
	SPI_ReadByte();
	// send 8 clocks to setup for next operation
	SPI_ReadByte();
	SPI_DeassertSS();

	return SD_ERROR_NONE;
}


uint16_t SD_WriteBlock(uint32_t startBlock, uint8_t *data, uint16_t size)
{
	uint8_t resp;
	uint16_t i, timeout = 0;

	resp = SD_Command(SD_WRITE_BLOCK, startBlock);
	if (resp != 0)
		return resp;

	SPI_AssertSS();
	SPI_SendByte(SD_START_BLOCK_TOKEN);

	for (i = 0; i < size; i++)
		SPI_SendByte(data[i]);

	// send fake CRC
	SPI_SendByte(0xFF);
	SPI_SendByte(0xFF);

	resp = SPI_ReadByte();
	if ( (resp & 0x1f) != 0x05)
	{
		SPI_DeassertSS();
		return resp;
	}

	// wait for write to complete
	while(!SPI_ReadByte())
		if (timeout++ > 0xFFFE)
		{
			SPI_DeassertSS();
			return SD_ERROR_TIMEOUT_WRITE;
		}

	SPI_DeassertSS();
	SPI_ReadByte();
	SPI_AssertSS();

	while(!SPI_ReadByte())
		if (timeout++ > 0xFFFE)
		{
			SPI_DeassertSS();
			return SD_ERROR_TIMEOUT_WRITE;
		}

	SPI_DeassertSS();
	return SD_ERROR_NONE;
}
