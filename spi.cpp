/* spi.cpp : simple SPI communication class
 * Copyright (C) 2019 Felix Haschke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"

// Initialize the SPI as master
void SPI_Init()
{
	// make the MOSI and SCK pins outputs
	SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK);

	// make sure the MISO pin is input
	SPI_DDR &= ~(1 << SPI_MISO);

	// set up the SPI module: SPI enabled, MSB first, master mode,
	//  clock polarity and phase = 0, F_osc/16
	SPI_SPCR = (1 << SPI_SPE) | (1 << SPI_MSTR) /*| (1 << SPIE)*/; // | ( 1 << SPI_SPR0 );
	SPI_SPSR = (1 << SPI2X);                     // set double SPI speed for F_osc/2
}

// Transfer a byte of data
void SPI_SendByte(uint8_t data)
{
	// Start transmission
	SPI_SPDR = data;

	// Wait for the transmission to complete
	spi_wait();
}
