#include "include/spi.h"
#include "include/AT26DF081A.h"

#include <avr/io.h>
#include <util/delay.h>

void flashWriteAddress( uint32_t addr, uint8_t sendDummy )
{
	uint8_t data;
	// send ADDR, MSB first
	data = (uint8_t)((addr >> 16) & 0x000000FF);
	SPI_SendByte(data);
	data = (uint8_t)((addr >> 8) & 0x000000FF);
	SPI_SendByte(data);
	data = (uint8_t)(addr & 0x000000FF);
	SPI_SendByte(data);
	
	// send don't care byte
	if( 1 == sendDummy )
		SPI_SendByte(0x00);

}


void flashReadArray( uint32_t addr, uint8_t *buffer, uint32_t numBytes )
{
	uint32_t i = 0;
	// assert the Slave Select line
	SPI_AssertSS();

	// send READ_ARRAY command via SPI
	SPI_SendByte( READ_ARRAY_LOW_FREQUENCY );

	// send memory address to start reading at
	flashWriteAddress( addr, 0 );

	// read in numBytes bytes
	for(i = 0; i < numBytes; i++)
	{
		*(buffer+i) = SPI_SendByte(0x00);
	}

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashWriteArray( uint32_t addr, uint8_t *buffer, uint16_t numBytes )
{
	uint16_t i = 0;

	if( numBytes > 256 )
		return;
	
	// assert the Slave Select line
	SPI_AssertSS();
	
	// send the write command opcode
	SPI_SendByte( BYTE_PAGE_PROGRAM );

	// send memory address to start writing at
	flashWriteAddress( addr, 0 );

	// write numBytes out through SPI
	for( i = 0; i < numBytes; i++ )
	{
		SPI_SendByte(buffer[i]);
	}

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashEraseBlock( uint32_t addr, uint8_t blockSize )
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the block erase command based on blockSize
	SPI_SendByte(blockSize);

	// send memory address to start block erase at
	flashWriteAddress( addr, 0 );

	// de-assert the slave select line
	SPI_DeassertSS();
}


void flashEraseChip( void )
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the chip erase command
	SPI_SendByte( CHIP_ERASE );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashWriteEnable()
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the write enable command
	SPI_SendByte( WRITE_ENABLE );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashWriteDisable()
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the write disable command
	SPI_SendByte( WRITE_DISABLE );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


uint8_t flashReadStatusRegister()
{
	uint8_t status = 0x00;

	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the write disable command
	SPI_SendByte( READ_STATUS_REGISTER );

	// read in the status register
	status = SPI_SendByte( 0x00 );

	// de-assert the Slave Select line
	SPI_DeassertSS();

	return status;
}


void flashWriteStatusRegister( uint8_t status )
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the write disable command
	SPI_SendByte( WRITE_STATUS_REGISTER );

	// write the status to status register
	SPI_SendByte( status );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashReadMfgDeviceID( uint8_t *mfgID, uint16_t *devID )
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the read dev id command
	SPI_SendByte( READ_MFG_AND_DEVICE_ID );
	
	// read in the MFG ID
	*mfgID = SPI_SendByte( 0x00 );
	// read in the high byte of DEV ID
	*((char *)devID) = SPI_SendByte( 0x00 );
	// read in the low byte of DEV ID
	*((char *)devID+1) = SPI_SendByte( 0x00 );
	// read in the extended dev ID info, which is 0x00 for this part
	SPI_SendByte( 0x00 );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashDeepSleep()
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the deep sleep command
	SPI_SendByte( DEEP_POWER_DOWN );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}


void flashWakeUp()
{
	// assert the Slave Select line
	SPI_AssertSS();

	// send the opcode for the wake from deep sleep command
	SPI_SendByte( RESUME_FROM_SLEEP );

	// de-assert the Slave Select line
	SPI_DeassertSS();
}

// flashReady returns 1 if flash is not busy, 0 otherwise
uint8_t flashReady( void )
{
	return !(flashReadStatusRegister() & 0x01);
}
