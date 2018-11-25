// spi.h


#ifndef _SPI_CLARKDAWG
#define _SPI_CLARKDAWG

#include <inttypes.h>

// SPI Definitions
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_MISO	PORTB4
#define SPI_MOSI	PORTB3
#define SPI_SCK		PORTB5
#define SPI_SS		PORTB2

#define SPI_SPCR	SPCR
#define SPI_SPDR	SPDR
#define SPI_SPSR	SPSR
#define SPI_SPIF	SPIF
#define SPI_SPE		SPE
#define SPI_MSTR	MSTR
#define SPI_SPR0	SPR0
#define SPI_SPR1	SPR1


// Loop until any current SPI transmissions have completed
#define spi_wait()	while (!(SPI_SPSR & (1 << SPI_SPIF)));

// Initialize the SPI subsystem
void SPI_Init();

// Transfer a byte of data
uint8_t SPI_SendByte( uint8_t data );

// Read a byte of data
uint8_t SPI_ReadByte( void );

// Assert the SS line
void SPI_AssertSS( void );

// Deassert the SS line
void SPI_DeassertSS( void );

#endif // _SPI_CLARKDAWG
