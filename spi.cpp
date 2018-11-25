// spi.c
//
// SPI master routines were pulled from the Atmel ATMega168 datasheet.

#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"

// Initialize the SPI as master
void SPI_Init()
{
	// make the MOSI, SCK, and SS pins outputs
	SPI_DDR |= ( 1 << SPI_MOSI ) | ( 1 << SPI_SCK ) | ( 1 << SPI_SS );

	// make sure the MISO pin is input
	SPI_DDR &= ~( 1 << SPI_MISO );

	// set up the SPI module: SPI enabled, MSB first, master mode,
	//  clock polarity and phase = 0, F_osc/16
	SPI_SPCR = ( 1 << SPI_SPE ) | ( 1 << SPI_MSTR ) /*| (1 << SPIE)*/;// | ( 1 << SPI_SPR0 );
	SPI_SPSR = (1 << SPI2X) /*| (1 << SPIF)*/;     // set double SPI speed for F_osc/2
}

// Transfer a byte of data
uint8_t SPI_SendByte( uint8_t data )
{
	// Start transmission
//	   SPI_SPCR |= (1<<SPI_SPE);
	SPI_SPDR = data;

	// Wait for the transmission to complete
	spi_wait();
//	SPI_SPCR &= ~(1<<SPI_SPE);
	// return the byte received from the slave
	return SPI_SPDR;
}


// Transfer a byte of data
uint8_t SPI_ReadByte( void )
{
	// Start transmission
	SPI_SPDR = 0xFF;

	// Wait for the transmission to complete
	spi_wait();

	// return the byte received from the slave
	return SPI_SPDR;
}


// Assert the SS line
void SPI_AssertSS()
{
	SPI_PORT &= ~(1 << SPI_SS);
}

// Deassert the SS line
void SPI_DeassertSS()
{
	SPI_PORT |= (1 << SPI_SS);
}
