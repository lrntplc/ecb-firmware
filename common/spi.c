/*
 *  Atmega8 SPI driver
 *
 *  Copyright 2016 - Laurentiu Palcu <lpalcu@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "util.h"

static volatile uint8_t transfer_done;

ISR(SPI_STC_vect)
{
	transfer_done = true;
}

void spi_master_init()
{

	/* Set MOSI, SCK and SS as Output */
	DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2);

	/*
	 * Enable SPI, Set as Master
	 * Prescaler: Fosc/4, Enable Interrupts
	 * The MOSI, SCK pins are as per ATMega8
	 */
	SPCR = (1 << SPE) | (1 << MSTR) | ( 1 << SPIE );
}

void spi_master_cs_set()
{
	PORTB &= ~_BV(PB2);

	_delay_us(1);
}

void spi_master_cs_clear()
{
	_delay_us(1);

	PORTB |= _BV(PB2);
}

void spi_master_send(uint8_t data)
{
	SPDR = data;

	transfer_done = false;

	while (!transfer_done)
		;
}
