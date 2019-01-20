/* spi.h : SPI communication library
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */
#pragma once

#include <inttypes.h>

// SPI Definitions
#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SPI_MISO PORTB4
#define SPI_MOSI PORTB3
#define SPI_SCK PORTB5
#define SPI_SS PORTB2

#define SPI_SPCR SPCR
#define SPI_SPDR SPDR
#define SPI_SPSR SPSR
#define SPI_SPIF SPIF
#define SPI_SPE SPE
#define SPI_MSTR MSTR
#define SPI_SPR0 SPR0
#define SPI_SPR1 SPR1

// Loop until any current SPI transmissions have completed
#define spi_wait()                       \
	while (!(SPI_SPSR & (1 << SPI_SPIF))) \
	   ;

// Initialize the SPI subsystem
void SPI_Init();

// Transfer a byte of data
void SPI_SendByte(uint8_t data);
